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
#include "dialogsearchprocess.h"
#include "ui_dialogsearchprocess.h"

DialogSearchProcess::DialogSearchProcess(QWidget *pParent, QIODevice *pDevice, SearchProcess::SEARCHDATA *pSearchData) :
    QDialog(pParent),
    ui(new Ui::DialogSearchProcess)
{
    ui->setupUi(this);

    this->g_pDevice=pDevice;
    this->g_pSearchData=pSearchData;

    g_pSearch=new SearchProcess;
    g_pThread=new QThread;

    g_pSearch->moveToThread(g_pThread);

    connect(g_pThread, SIGNAL(started()), g_pSearch, SLOT(process()));
    connect(g_pSearch, SIGNAL(completed(qint64)), this, SLOT(onCompleted(qint64)));
    connect(g_pSearch, SIGNAL(errorMessage(QString)), this, SLOT(errorMessage(QString)));
    connect(g_pSearch, SIGNAL(progressValueChanged(qint32)), this, SLOT(progressValueChanged(qint32)));
    connect(g_pSearch, SIGNAL(progressValueMinimum(qint32)), this, SLOT(progressValueMinimum(qint32)));
    connect(g_pSearch, SIGNAL(progressValueMaximum(qint32)), this, SLOT(progressValueMaximum(qint32)));

    g_pSearch->setData(pDevice,pSearchData);
    g_pThread->start();
}

DialogSearchProcess::~DialogSearchProcess()
{
    g_pSearch->stop();

    g_pThread->quit();
    g_pThread->wait();

    delete ui;

    delete g_pThread;
    delete g_pSearch;
}

void DialogSearchProcess::on_pushButtonCancel_clicked()
{
    g_pSearch->stop();
}

void DialogSearchProcess::errorMessage(QString sText)
{
    QMessageBox::critical(this,tr("Error"),sText);
}

void DialogSearchProcess::onCompleted(qint64 nElapsed)
{
    Q_UNUSED(nElapsed)

    if(g_pSearchData->nResultOffset!=-1)
    {
        done(Accepted);
    }
    else
    {
        done(Rejected);
    }
}

void DialogSearchProcess::progressValueChanged(qint32 nValue)
{
    ui->progressBarSearch->setValue(nValue);
}

void DialogSearchProcess::progressValueMinimum(qint32 nValue)
{
    ui->progressBarSearch->setMinimum(nValue);
}

void DialogSearchProcess::progressValueMaximum(qint32 nValue)
{
    ui->progressBarSearch->setMaximum(nValue);
}
