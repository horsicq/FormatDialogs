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
#include "dialogsearchstrings.h"
#include "ui_dialogsearchstrings.h"

DialogSearchStrings::DialogSearchStrings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSearchStrings)
{
    ui->setupUi(this);

    pHandleStrings=new SearchStrings;
    pHandleModel=new SearchStrings;
    pThreadSearch=new QThread;
    pThreadModel=new QThread;

    pHandleStrings->moveToThread(pThreadSearch);
    pHandleModel->moveToThread(pThreadModel);

    connect(pThreadSearch, SIGNAL(started()), pHandleStrings, SLOT(processSearch()));
    connect(pHandleStrings, SIGNAL(completed(qint64)), this, SLOT(onCompleted(qint64)));
    connect(pHandleStrings, SIGNAL(errorMessage(QString)), this, SLOT(errorMessage(QString)));
    connect(pHandleStrings, SIGNAL(progressValue(qint32)), this, SLOT(progressValue(qint32)));

    connect(pThreadModel, SIGNAL(started()), pHandleModel, SLOT(processModel()));
    connect(pHandleModel, SIGNAL(completed(qint64)), this, SLOT(onCompleted(qint64)));
    connect(pHandleModel, SIGNAL(errorMessage(QString)), this, SLOT(errorMessage(QString)));
    connect(pHandleModel, SIGNAL(progressValue(qint32)), this, SLOT(progressValue(qint32)));

    ui->progressBar->setMaximum(100);
    ui->progressBar->setMinimum(0);
}

DialogSearchStrings::~DialogSearchStrings()
{
    pHandleStrings->stop();

    pThreadSearch->quit();
    pThreadSearch->wait();

    pThreadModel->quit();
    pThreadModel->wait();

    delete ui;

    delete pThreadSearch;
    delete pThreadModel;
    delete pHandleStrings;
}

void DialogSearchStrings::processSearch(QIODevice *pDevice, QList<SearchStrings::RECORD> *pListRecords, SearchStrings::OPTIONS *pOptions)
{
    setWindowTitle(tr("Search strings"));

    pHandleStrings->setSearchData(pDevice,pListRecords,pOptions);
    pThreadSearch->start();
}

void DialogSearchStrings::processModel(QList<SearchStrings::RECORD> *pListRecords, QStandardItemModel **ppModel, SearchStrings::OPTIONS *pOptions)
{
    setWindowTitle(tr("Create view model"));

    pHandleModel->setModelData(pListRecords,ppModel,pOptions);
    pThreadModel->start();
}

void DialogSearchStrings::on_pushButtonCancel_clicked()
{
    pHandleStrings->stop();
}

void DialogSearchStrings::errorMessage(QString sText)
{
    QMessageBox::critical(this,tr("Error"),sText);
}

void DialogSearchStrings::onCompleted(qint64 nElapsed)
{
    Q_UNUSED(nElapsed)

    this->close();
}

void DialogSearchStrings::progressValue(qint32 nValue)
{
    ui->progressBar->setValue(nValue);
}
