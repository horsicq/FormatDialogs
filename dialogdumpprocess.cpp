// copyright (c) 2019-2021 hors<horsicq@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include "dialogdumpprocess.h"
#include "ui_dialogdumpprocess.h"

DialogDumpProcess::DialogDumpProcess(QWidget *pParent, QIODevice *pDevice, qint64 nOffset, qint64 nSize, QString sFileName, DumpProcess::DT dumpType) :
    QDialog(pParent),
    ui(new Ui::DialogDumpProcess)
{
    ui->setupUi(this);

    g_pDump=new DumpProcess;
    g_pThread=new QThread;

    g_pDump->moveToThread(g_pThread);

    connect(g_pThread, SIGNAL(started()), g_pDump, SLOT(process()));
    connect(g_pDump, SIGNAL(completed(qint64)), this, SLOT(onCompleted(qint64)));
    connect(g_pDump, SIGNAL(errorMessage(QString)), this, SLOT(errorMessage(QString)));
    connect(g_pDump, SIGNAL(progressValueChanged(qint32)), this, SLOT(progressValueChanged(qint32)));
    connect(g_pDump, SIGNAL(progressValueMinimum(qint32)), this, SLOT(progressValueMinimum(qint32)));
    connect(g_pDump, SIGNAL(progressValueMaximum(qint32)), this, SLOT(progressValueMaximum(qint32)));

    g_pDump->setData(pDevice,nOffset,nSize,sFileName,dumpType);
    g_pThread->start();
}

DialogDumpProcess::~DialogDumpProcess()
{
    g_pDump->stop();

    g_pThread->quit();
    g_pThread->wait();

    delete ui;

    delete g_pThread;
    delete g_pDump;
}

void DialogDumpProcess::on_pushButtonCancel_clicked()
{
    g_pDump->stop();
}

void DialogDumpProcess::errorMessage(QString sText)
{
    QMessageBox::critical(XOptions::getMainWidget(this),tr("Error"),sText);
}

void DialogDumpProcess::onCompleted(qint64 nElapsed)
{
    Q_UNUSED(nElapsed)

    this->close();
}

void DialogDumpProcess::progressValueChanged(qint32 nValue)
{
    ui->progressBar->setValue(nValue);
}

void DialogDumpProcess::progressValueMaximum(qint32 nValue)
{
    ui->progressBar->setMaximum(nValue);
}

void DialogDumpProcess::progressValueMinimum(qint32 nValue)
{
    ui->progressBar->setMinimum(nValue);
}
