// copyright (c) 2019 hors<horsicq@gmail.com>
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
#include "dialogdump.h"
#include "ui_dialogdump.h"

DialogDump::DialogDump(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDump)
{
    ui->setupUi(this);

    pDump=new Dump;
    pThread=new QThread;

    pDump->moveToThread(pThread);

    connect(pThread, SIGNAL(started()), pDump, SLOT(process()));
    connect(pDump, SIGNAL(completed(qint64)), this, SLOT(onCompleted(qint64)));
    connect(pDump, SIGNAL(errorMessage(QString)), this, SLOT(errorMessage(QString)));
    connect(pDump, SIGNAL(progressValue(qint32)), this, SLOT(progressValue(qint32)));

    ui->progressBar->setMaximum(100);
    ui->progressBar->setMinimum(0);
}

void DialogDump::dumpToFile(QIODevice *pDevice, qint64 nOffset, qint64 nSize, QString sFileName)
{
    pDump->setData(pDevice,nOffset,nSize,sFileName);
    pThread->start();
}

DialogDump::~DialogDump()
{
    pDump->stop();

    pThread->quit();
    pThread->wait();

    delete ui;

    delete pThread;
    delete pDump;
}

void DialogDump::on_pushButtonCancel_clicked()
{
    pDump->stop();
}

void DialogDump::errorMessage(QString sText)
{
    QMessageBox::critical(this,tr("Error"),sText);
}

void DialogDump::onCompleted(qint64 nElapsed)
{
    Q_UNUSED(nElapsed)

    this->close();
}

void DialogDump::progressValue(qint32 nValue)
{
    ui->progressBar->setValue(nValue);
}
