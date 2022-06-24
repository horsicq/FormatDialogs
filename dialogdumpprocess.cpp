/* Copyright (c) 2019-2022 hors<horsicq@gmail.com>
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
#include "ui_dialogdumpprocess.h"

DialogDumpProcess::DialogDumpProcess(QWidget *pParent):
    XDialogProcess(pParent),
    ui(new Ui::DialogDumpProcess)
{
    ui->setupUi(this);

    g_pDump=nullptr;
    g_pThread=nullptr;

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(1000);
}

DialogDumpProcess::DialogDumpProcess(QWidget *pParent,QIODevice *pDevice,qint64 nOffset,qint64 nSize,QString sFileName,DumpProcess::DT dumpType) :
    DialogDumpProcess(pParent)
{
    g_pDump=new DumpProcess;
    g_pThread=new QThread;

    g_pDump->moveToThread(g_pThread);

    connect(g_pThread,SIGNAL(started()),g_pDump,SLOT(process()));
    connect(g_pDump,SIGNAL(completed(qint64)),this,SLOT(onCompleted(qint64)));
    connect(g_pDump,SIGNAL(errorMessage(QString)),this,SLOT(errorMessage(QString)));

    g_pDump->setData(pDevice,nOffset,nSize,sFileName,dumpType,getPdStruct());
    g_pThread->start();
}

DialogDumpProcess::~DialogDumpProcess()
{
    stop();

    g_pThread->quit();
    g_pThread->wait();

    delete ui;

    delete g_pThread;
    delete g_pDump;
}

void DialogDumpProcess::on_pushButtonCancel_clicked()
{
    stop();
}

void DialogDumpProcess::_timerSlot()
{
    if(getPdStruct()->pdRecord.nTotal)
    {
        ui->progressBar->setValue((getPdStruct()->pdRecord.nCurrent*1000)/(getPdStruct()->pdRecord.nTotal));
    }
}
