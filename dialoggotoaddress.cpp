/* Copyright (c) 2019-2021 hors<horsicq@gmail.com>
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
#include "dialoggotoaddress.h"
#include "ui_dialoggotoaddress.h"

DialogGoToAddress::DialogGoToAddress(QWidget *pParent, XBinary::_MEMORY_MAP *pMemoryMap, TYPE type) :
    XShortcutsDialog(pParent),
    ui(new Ui::DialogGoToAddress)
{
    ui->setupUi(this);

    g_nMinValue=0;
    g_nMaxValue=0;
    g_nValue=0;
    g_pMemoryMap=pMemoryMap;
    g_type=type;

    adjustTitle(type);
}

DialogGoToAddress::DialogGoToAddress(QWidget *pParent, qint64 nMinValue, qint64 nMaxValue, DialogGoToAddress::TYPE type) :
    XShortcutsDialog(pParent),
    ui(new Ui::DialogGoToAddress)
{
    ui->setupUi(this);

    g_pMemoryMap=nullptr;
    g_nMinValue=nMinValue;
    g_nMaxValue=nMaxValue;
    g_type=type;
    g_nValue=0;

    adjustTitle(type);
}

DialogGoToAddress::~DialogGoToAddress()
{
    delete ui;
}

qint64 DialogGoToAddress::getValue()
{
    return g_nValue;
}

void DialogGoToAddress::adjustTitle(DialogGoToAddress::TYPE type)
{
    QString sTitle="";
    QString sValue="";

    if(type==TYPE_ADDRESS)
    {
        sTitle=tr("Virtual address"); // mb TODO rename to "Address" or add extra type
        sValue=tr("Value");
    }
    else if(type==TYPE_OFFSET)
    {
        sTitle=tr("Offset");
        sValue=tr("Value");
    }
    else if(type==TYPE_RELADDRESS)
    {
        sTitle=tr("Relative virtual address");
        sValue=tr("Value");
    }

    setWindowTitle(sTitle);
    ui->groupBoxValue->setTitle(sValue);
}

void DialogGoToAddress::on_pushButtonCancel_clicked()
{
    reject();
}

void DialogGoToAddress::on_pushButtonOK_clicked()
{
    qint64 nValue=(qint64)ui->lineEditValue->getValue();

    bool bValid=false;

    if(g_pMemoryMap)
    {
        if(g_type==TYPE_ADDRESS)
        {
            bValid=XBinary::isAddressValid(g_pMemoryMap,nValue);
        }
        else if(g_type==TYPE_OFFSET)
        {
            bValid=XBinary::isOffsetValid(g_pMemoryMap,nValue);
        }
        else if(g_type==TYPE_RELADDRESS)
        {
            bValid=XBinary::isRelAddressValid(g_pMemoryMap,nValue);
        }
    }
    else
    {
        if((nValue>=g_nMinValue)&&(nValue<=g_nMaxValue))
        {
            bValid=true;
        }
    }

    if(bValid)
    {
        this->g_nValue=nValue;
        accept();
    }
    else
    {
        ui->labelStatus->setText(tr("Invalid"));
    }
}
