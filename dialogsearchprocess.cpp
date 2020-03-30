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
#include "dialogsearchprocess.h"
#include "ui_dialogsearchprocess.h"

DialogSearchProcess::DialogSearchProcess(QWidget *parent, QIODevice *pDevice, SearchProcess::SEARCHDATA *pSearchData) :
    QDialog(parent),
    ui(new Ui::DialogSearchProcess)
{
    ui->setupUi(this);

    this->pDevice=pDevice;
    this->pSearchData=pSearchData;
}

DialogSearchProcess::~DialogSearchProcess()
{
    delete ui;
}

void DialogSearchProcess::on_pushButtonCancel_clicked()
{

}

void DialogSearchProcess::errorMessage(QString sText)
{
    QMessageBox::critical(this,tr("Error"),sText);
}

void DialogSearchProcess::completed(qint64 nElapsed)
{
    // TODO
}

void DialogSearchProcess::progressValueChanged(qint32 nValue)
{
    ui->progressBarSearch->setValue(nValue);
}

void DialogSearchProcess::progressValueMaximum(qint32 nValue)
{
    ui->progressBarSearch->setMaximum(nValue);
}
