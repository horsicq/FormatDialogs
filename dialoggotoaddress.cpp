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
#include "dialoggotoaddress.h"
#include "ui_dialoggotoaddress.h"

DialogGoToAddress::DialogGoToAddress(QWidget *parent, XBinary::_MEMORY_MAP *pMemoryMap,TYPE type) :
    QDialog(parent),
    ui(new Ui::DialogGoToAddress)
{
    ui->setupUi(this);
    this->pMemoryMap=pMemoryMap;
    this->type=type;
    nValue=0;

    QString sCaption="";

    if(type==TYPE_ADDRESS)
    {
        sCaption=tr("Address");
    }
    else if(type==TYPE_OFFSET)
    {
        sCaption=tr("Offset");
    }

    setWindowTitle(QString("%1 %2").arg(tr("Go to")).arg(sCaption));
    ui->groupBoxValue->setTitle(sCaption);
}

DialogGoToAddress::~DialogGoToAddress()
{
    delete ui;
}

qint64 DialogGoToAddress::getValue()
{
    return nValue;
}

void DialogGoToAddress::on_pushButtonCancel_clicked()
{
    reject();
}

void DialogGoToAddress::on_pushButtonOK_clicked()
{
    qint64 nValue=(qint64)ui->lineEditValue->getValue();

    bool bValid=false;

    if(type==TYPE_ADDRESS)
    {
        bValid=XBinary::isAddressValid(pMemoryMap,nValue);
    }
    else if(type==TYPE_OFFSET)
    {
        bValid=XBinary::isOffsetValid(pMemoryMap,nValue);
    }

    if(bValid)
    {
        this->nValue=nValue;
        accept();
    }
    else
    {
        ui->labelStatus->setText(tr("Invalid"));
    }
}
