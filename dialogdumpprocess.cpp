/* Copyright (c) 2019-2025 hors<horsicq@gmail.com>
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
#include "dialogdumpprocess.h"

DialogDumpProcess::DialogDumpProcess(QWidget *pParent) : XDialogProcess(pParent)
{
    g_pDump = new DumpProcess;
    g_pThread = new QThread;

    g_pDump->moveToThread(g_pThread);

    connect(g_pThread, SIGNAL(started()), g_pDump, SLOT(process()));
    connect(g_pDump, SIGNAL(completed(qint64)), this, SLOT(onCompleted(qint64)));
    connect(g_pDump, SIGNAL(errorMessage(QString)), this, SLOT(errorMessageSlot(QString)));

    setWindowTitle(tr("Dump"));
}

DialogDumpProcess::~DialogDumpProcess()
{
    stop();
    waitForFinished();

    g_pThread->quit();
    g_pThread->wait();

    delete g_pThread;
    delete g_pDump;
}

void DialogDumpProcess::setData(QIODevice *pDevice, qint64 nOffset, qint64 nSize, const QString &sFileName, DumpProcess::DT dumpType)
{
    QList<DumpProcess::RECORD> listRecords;

    DumpProcess::RECORD record = {};

    record.nOffset = nOffset;
    record.nSize = nSize;
    record.sFileName = sFileName;

    listRecords.append(record);

    QString sJsonFile = sFileName + ".patch.json";

    g_pDump->setData(pDevice, listRecords, dumpType, sJsonFile, getPdStruct());
    g_pThread->start();
}

void DialogDumpProcess::setData(QIODevice *pDevice, const QList<DumpProcess::RECORD> &listRecords, DumpProcess::DT dumpType, const QString &sJsonFileName)
{
    g_pDump->setData(pDevice, listRecords, dumpType, sJsonFileName, getPdStruct());
    g_pThread->start();
}

void DialogDumpProcess::setData(QIODevice *pDevice, DumpProcess::RECORD record, DumpProcess::DT dumpType)
{
    QList<DumpProcess::RECORD> listRecords;

    listRecords.append(record);

    g_pDump->setData(pDevice, listRecords, dumpType, record.sFileName + ".patch.json", getPdStruct());
    g_pThread->start();
}

void DialogDumpProcess::setData(QIODevice *pDevice, DumpProcess::DT dumpType, const QString &sJsonFileName)
{
    g_pDump->setData(pDevice, dumpType, sJsonFileName, getPdStruct());
    g_pThread->start();
}
#ifdef USE_XPROCESS
void DialogDumpProcess::setData(X_ID nProcessID, XADDR nAddress, qint64 nSize, DumpProcess::DT dumpType, const QString &sFileName)
{
    g_pDump->setData(nProcessID, nAddress, nSize, dumpType, sFileName, sFileName + ".dmp.json", getPdStruct());
    g_pThread->start();
}
#endif
#ifdef USE_XPROCESS
#ifdef Q_OS_WIN
void DialogDumpProcess::setData(X_ID nProcessID, XADDR nAddress, qint64 nSize, DumpProcess::DT dumpType, const QString &sFileName,
                                const XPE::FIXDUMP_OPTIONS &fixDumpOptions, const QByteArray &baHeaders)
{
    g_pDump->setData(nProcessID, nAddress, nSize, dumpType, sFileName, sFileName + ".dmp.json", fixDumpOptions, baHeaders, getPdStruct());
    g_pThread->start();
}
#endif
#endif
#ifdef USE_XPROCESS
#ifdef Q_OS_LINUX
void DialogDumpProcess::setData(X_ID nProcessID, XADDR nAddress, qint64 nSize, DumpProcess::DT dumpType, const QString &sFileName,
                                const XELF::FIXDUMP_OPTIONS &fixDumpOptions, const QByteArray &baHeaders)
{
    g_pDump->setData(nProcessID, nAddress, nSize, dumpType, sFileName, sFileName + ".dmp.json", fixDumpOptions, baHeaders, getPdStruct());
    g_pThread->start();
}
#endif
#endif
