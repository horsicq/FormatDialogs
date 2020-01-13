// copyright (c) 2019-2020 hors<horsicq@gmail.com>
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

DialogDumpProcess::DialogDumpProcess(QWidget *parent, QIODevice *pDevice, qint64 nOffset, qint64 nSize, QString sFileName) :
    QDialog(parent),
    ui(new Ui::DialogDumpProcess)
{
    ui->setupUi(this);

    pDump=new DumpProcess;
    pThread=new QThread;

    pDump->moveToThread(pThread);

    connect(pThread, SIGNAL(started()), pDump, SLOT(process()));
    connect(pDump, SIGNAL(completed(qint64)), this, SLOT(onCompleted(qint64)));
    connect(pDump, SIGNAL(errorMessage(QString)), this, SLOT(errorMessage(QString)));
    connect(pDump, SIGNAL(progressValue(qint32)), this, SLOT(progressValue(qint32)));

    ui->progressBar->setMaximum(100);
    ui->progressBar->setMinimum(0);

    pDump->setData(pDevice,nOffset,nSize,sFileName);
    pThread->start();
}

DialogDumpProcess::~DialogDumpProcess()
{
    pDump->stop();

    pThread->quit();
    pThread->wait();

    delete ui;

    delete pThread;
    delete pDump;
}

void DialogDumpProcess::on_pushButtonCancel_clicked()
{
    pDump->stop();
}

void DialogDumpProcess::errorMessage(QString sText)
{
    QMessageBox::critical(this,tr("Error"),sText);
}

void DialogDumpProcess::onCompleted(qint64 nElapsed)
{
    Q_UNUSED(nElapsed)

    this->close();
}

void DialogDumpProcess::progressValue(qint32 nValue)
{
    ui->progressBar->setValue(nValue);
}
