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
}

void DumpProcess::setData(QIODevice *pDevice, QList<RECORD> listRecords, DT dumpType, XBinary::PDSTRUCT *pPdStruct)
{
    this->g_pDevice = pDevice;
    this->g_listRecords = listRecords;
    this->g_dumpType = dumpType;
    this->g_pPdStruct = pPdStruct;
}

void DumpProcess::process()
{
    QElapsedTimer scanTimer;
    scanTimer.start();

    XBinary binary(g_pDevice);

    connect(&binary, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));

    qint32 nNumberOfRecords = g_listRecords.count();

    qint32 _nFreeIndex = XBinary::getFreeIndex(g_pPdStruct);
    XBinary::setPdStructInit(g_pPdStruct, _nFreeIndex, nNumberOfRecords);

    for (qint32 i = 0; (i < nNumberOfRecords) && (!(g_pPdStruct->bIsStop)); i++) {
        if (g_dumpType == DT_OFFSET) {
            binary.dumpToFile(g_listRecords.at(i).sFileName, g_listRecords.at(i).nOffset, g_listRecords.at(i).nSize, g_pPdStruct);
        }

        XBinary::setPdStructCurrentIncrement(g_pPdStruct, _nFreeIndex);
    }

    XBinary::setPdStructFinished(g_pPdStruct, _nFreeIndex);

    emit completed(scanTimer.elapsed());
}
