/* Copyright (c) 2019-2023 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "dumpprocess.h"

DumpProcess::DumpProcess(QObject *pParent) : QObject(pParent)
{
    g_pPdStruct = nullptr;
#ifdef USE_XPROCESS
    g_nProcessID = 0;
#endif
}

void DumpProcess::setData(QIODevice *pDevice, QList<RECORD> listRecords, DT dumpType, QString sJsonFileName, XBinary::PDSTRUCT *pPdStruct)
{
    this->g_pDevice = pDevice;
    this->g_listRecords = listRecords;
    this->g_dumpType = dumpType;
    this->g_sJsonFileName = sJsonFileName;
    this->g_pPdStruct = pPdStruct;
}

void DumpProcess::setData(QIODevice *pDevice, DT dumpType, QString sJsonFileName, XBinary::PDSTRUCT *pPdStruct)
{
    this->g_pDevice = pDevice;
    this->g_dumpType = dumpType;
    this->g_sJsonFileName = sJsonFileName;
    this->g_pPdStruct = pPdStruct;
}
#ifdef USE_XPROCESS
void DumpProcess::setData(X_ID nProcessID, DT dumpType, QString sFileName, QString sJsonFileName, XBinary::PDSTRUCT *pPdStruct)
{
    this->g_nProcessID = nProcessID;
    this->g_dumpType = dumpType;
    this->g_sFileName = sFileName;
    this->g_sJsonFileName = sJsonFileName;
    this->g_pPdStruct = pPdStruct;
}
#endif
void DumpProcess::process()
{
    QElapsedTimer scanTimer;
    scanTimer.start();

    if (g_dumpType == DT_DUMP_DEVICE_OFFSET) {
        XBinary binary(g_pDevice);

        connect(&binary, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));

        qint32 nNumberOfRecords = g_listRecords.count();

        qint32 _nFreeIndex = XBinary::getFreeIndex(g_pPdStruct);
        XBinary::setPdStructInit(g_pPdStruct, _nFreeIndex, nNumberOfRecords);

        QJsonArray jsArray;

        for (qint32 i = 0; (i < nNumberOfRecords) && (!(g_pPdStruct->bIsStop)); i++) {
            QString _sFileName = g_listRecords.at(i).sFileName;
            qint64 _nOffset = g_listRecords.at(i).nOffset;
            qint64 _nSize = g_listRecords.at(i).nSize;

            binary.dumpToFile(_sFileName, _nOffset, _nSize, g_pPdStruct);

            QJsonObject jsObject;
            jsObject.insert("offset", _nOffset);
            jsObject.insert("size", _nSize);
            jsObject.insert("filename", _sFileName);

            jsArray.append(jsObject);

            XBinary::setPdStructCurrentIncrement(g_pPdStruct, _nFreeIndex);
        }

        QJsonDocument saveFormat(jsArray);

        XBinary::writeToFile(g_sJsonFileName, saveFormat.toJson(QJsonDocument::Indented));

        XBinary::setPdStructFinished(g_pPdStruct, _nFreeIndex);
    } else if (g_dumpType == DT_PATCH_DEVICE_OFFSET) {
        XBinary binary(g_pDevice);

        connect(&binary, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));

        QByteArray baJson = XBinary::readFile(g_sJsonFileName);

        QJsonDocument jsDoc = QJsonDocument::fromJson(baJson);

        if (jsDoc.isArray()) {
            qint32 nNumberOfRecords = jsDoc.array().count();

            qint32 _nFreeIndex = XBinary::getFreeIndex(g_pPdStruct);
            XBinary::setPdStructInit(g_pPdStruct, _nFreeIndex, nNumberOfRecords);

            for (qint32 i = 0; (i < nNumberOfRecords) && (!(g_pPdStruct->bIsStop)); i++) {
                QJsonObject jsObject = jsDoc.array().at(i).toObject();

                QString _sFileName = jsObject.value("filename").toString();
                qint64 _nOffset = jsObject.value("offset").toVariant().toLongLong();
                qint64 _nSize = jsObject.value("size").toVariant().toLongLong();
                qint64 _nFileSize = QFileInfo(_sFileName).size();

                if (( _nOffset > _nFileSize) || (_nOffset <= 0)) {
                    emit errorMessage(tr("Invalid offset"));
                    break;
                }

                if ((_nOffset + _nSize > _nFileSize) || (_nSize <= 0) || (_nFileSize < _nSize)) {
                    emit errorMessage(tr("Invalid size"));
                    break;
                }

                // TODO Check params

                QString sFileMD5 = XBinary::getHash(XBinary::HASH_MD5, _sFileName, g_pPdStruct);
                QString sOriginMD5 = binary.getHash(XBinary::HASH_MD5, _nOffset, _nSize, g_pPdStruct);

                if (sFileMD5 != sOriginMD5) {
                    if (!binary.patchFromFile(_sFileName, _nOffset, _nSize, g_pPdStruct)) {
                        emit errorMessage(QString("%1 :").arg(tr("Cannot read file"), _sFileName));
                        break;
                    }
                }

                XBinary::setPdStructCurrentIncrement(g_pPdStruct, _nFreeIndex);
            }

            XBinary::setPdStructFinished(g_pPdStruct, _nFreeIndex);
        }
    }
#ifdef USE_XPROCESS
    else if (g_dumpType == DT_DUMP_PROCESS_USER_ID_RAW) {
        // TODO
    }
#endif

    emit completed(scanTimer.elapsed());
}
