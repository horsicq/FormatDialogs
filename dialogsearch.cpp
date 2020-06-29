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
#include "dialogsearch.h"
#include "ui_dialogsearch.h"

DialogSearch::DialogSearch(QWidget *parent, QIODevice *pDevice, SearchProcess::SEARCHDATA *pSearchData) :
    QDialog(parent),
    ui(new Ui::DialogSearch)
{
    ui->setupUi(this);

    const QSignalBlocker block1(ui->comboBoxEndianness);
    const QSignalBlocker block2(ui->lineEditValue);
    const QSignalBlocker block3(ui->tabWidgetSearch);
    const QSignalBlocker block4(ui->radioButtonChar);
    const QSignalBlocker block5(ui->radioButtonDouble);
    const QSignalBlocker block6(ui->radioButtonFloat);
    const QSignalBlocker block7(ui->radioButtonInt);
    const QSignalBlocker block8(ui->radioButtonInt64);
    const QSignalBlocker block9(ui->radioButtonShort);
    const QSignalBlocker block10(ui->radioButtonUchar);
    const QSignalBlocker block11(ui->radioButtonUint);
    const QSignalBlocker block12(ui->radioButtonUint64);
    const QSignalBlocker block13(ui->radioButtonUshort);

    this->pDevice=pDevice;
    this->pSearchData=pSearchData;

    ui->comboBoxSearchFrom->addItem(tr("Begin"));
    ui->comboBoxSearchFrom->addItem(tr("Cursor"));

    ui->comboBoxType->addItem(QString("ANSI"));
    ui->comboBoxType->addItem(QString("Unicode"));

    ui->comboBoxEndianness->addItem(QString("LE"));
    ui->comboBoxEndianness->addItem(QString("BE"));

    ui->plainTextEditString->setFocus();

    ui->tabWidgetSearch->setCurrentIndex(0);

    ui->lineEditValue->setText("0");

    ui->radioButtonUint->setChecked(true);

    ajustValue();
}

DialogSearch::~DialogSearch()
{
    delete ui;
}

void DialogSearch::on_pushButtonCancel_clicked()
{
    this->close();
}

void DialogSearch::on_pushButtonOK_clicked()
{
    if(ui->tabWidgetSearch->currentIndex()==0) // Strings
    {
        bool bMatchCase=ui->checkBoxMatchCase->isChecked();

        if(ui->comboBoxType->currentIndex()==0) // ANSI
        {
            if(bMatchCase)
            {
                pSearchData->type=SearchProcess::TYPE_ANSISTRING;
            }
            else
            {
                pSearchData->type=SearchProcess::TYPE_ANSISTRING_I;
            }
        }
        else if(ui->comboBoxType->currentIndex()==1) // UNICODE
        {
            if(bMatchCase)
            {
                pSearchData->type=SearchProcess::TYPE_UNICODESTRING;
            }
            else
            {
                pSearchData->type=SearchProcess::TYPE_UNICODESTRING_I;
            }
        }

        QString sText=ui->plainTextEditString->toPlainText();

        if(sText.length()>256) // TODO const
        {
            sText.resize(256);
        }

        pSearchData->variant=sText;
    }
    else if(ui->tabWidgetSearch->currentIndex()==1) // Signature
    {
        QString sText=ui->plainTextEditSignature->toPlainText();

        if(sText.length()>256) // TODO const
        {
            sText.resize(256);
        }

        pSearchData->type=SearchProcess::TYPE_SIGNATURE;
        pSearchData->variant=sText;
    }
    else if(ui->tabWidgetSearch->currentIndex()==2) // Value
    {
        pSearchData->bIsBigEndian=(ui->comboBoxEndianness->currentIndex()==1);

        pSearchData->variant=ui->lineEditValue->text();

        if(ui->radioButtonChar->isChecked())
        {
            pSearchData->type=SearchProcess::TYPE_VALUE_CHAR;
        }
        else if(ui->radioButtonUchar->isChecked())
        {
            pSearchData->type=SearchProcess::TYPE_VALUE_UCHAR;
        }
        else if(ui->radioButtonDouble->isChecked())
        {
            pSearchData->type=SearchProcess::TYPE_VALUE_DOUBLE;
        }
        else if(ui->radioButtonFloat->isChecked())
        {
            pSearchData->type=SearchProcess::TYPE_VALUE_FLOAT;
        }
        else if(ui->radioButtonInt->isChecked())
        {
            pSearchData->type=SearchProcess::TYPE_VALUE_INT;
        }
        else if(ui->radioButtonInt64->isChecked())
        {
            pSearchData->type=SearchProcess::TYPE_VALUE_INT64;
        }
        else if(ui->radioButtonShort->isChecked())
        {
            pSearchData->type=SearchProcess::TYPE_VALUE_SHORT;
        }
        else if(ui->radioButtonUchar->isChecked())
        {
            pSearchData->type=SearchProcess::TYPE_VALUE_UCHAR;
        }
        else if(ui->radioButtonUint->isChecked())
        {
            pSearchData->type=SearchProcess::TYPE_VALUE_UINT;
        }
        else if(ui->radioButtonUint64->isChecked())
        {
            pSearchData->type=SearchProcess::TYPE_VALUE_UINT64;
        }
        else if(ui->radioButtonUshort->isChecked())
        {
            pSearchData->type=SearchProcess::TYPE_VALUE_USHORT;
        }
    }

    DialogSearchProcess dsp(this,pDevice,pSearchData);

    done(dsp.exec());
}

void DialogSearch::on_tabWidgetSearch_currentChanged(int index)
{
    if(index==0) // Strings
    {
        ui->plainTextEditString->setFocus();
    }
    else if(index==1) // Signatures
    {
        ui->plainTextEditSignature->setFocus();
    }
}

void DialogSearch::on_lineEditValue_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)

    ajustValue();
}

void DialogSearch::on_comboBoxEndianness_currentIndexChanged(int index)
{
    Q_UNUSED(index)

    ajustValue();
}

void DialogSearch::on_radioButtonChar_toggled(bool checked)
{
    Q_UNUSED(checked)

    ajustValue();
}

void DialogSearch::on_radioButtonUchar_toggled(bool checked)
{
    Q_UNUSED(checked)

    ajustValue();
}

void DialogSearch::on_radioButtonShort_toggled(bool checked)
{
    Q_UNUSED(checked)

    ajustValue();
}

void DialogSearch::on_radioButtonUshort_toggled(bool checked)
{
    Q_UNUSED(checked)

    ajustValue();
}

void DialogSearch::on_radioButtonInt_toggled(bool checked)
{
    Q_UNUSED(checked)

    ajustValue();
}

void DialogSearch::on_radioButtonUint_toggled(bool checked)
{
    Q_UNUSED(checked)

    ajustValue();
}

void DialogSearch::on_radioButtonInt64_toggled(bool checked)
{
    Q_UNUSED(checked)

    ajustValue();
}

void DialogSearch::on_radioButtonUint64_toggled(bool checked)
{
    Q_UNUSED(checked)

    ajustValue();
}

void DialogSearch::on_radioButtonFloat_toggled(bool checked)
{
    Q_UNUSED(checked)

    ajustValue();
}

void DialogSearch::on_radioButtonDouble_toggled(bool checked)
{
    Q_UNUSED(checked)

    ajustValue();
}

void DialogSearch::ajustValue()
{
    QString sValue=ui->lineEditValue->text();
    QString sHex;

    bool bIsBigEndian=(ui->comboBoxEndianness->currentIndex()==1);

    if(ui->radioButtonChar->isChecked())
    {
        if(XBinary::checkString_int8(sValue))
        {
            sHex=XBinary::valueToHex((qint8)sValue.toShort(),bIsBigEndian);
        }
    }
    else if(ui->radioButtonUchar->isChecked())
    {
        if(XBinary::checkString_uint8(sValue))
        {
            sHex=XBinary::valueToHex((quint8)sValue.toUShort(),bIsBigEndian);
        }
    }
    else if(ui->radioButtonShort->isChecked())
    {
        if(XBinary::checkString_int16(sValue))
        {
            sHex=XBinary::valueToHex((qint16)sValue.toShort(),bIsBigEndian);
        }
    }
    else if(ui->radioButtonUshort->isChecked())
    {
        if(XBinary::checkString_uint16(sValue))
        {
            sHex=XBinary::valueToHex((quint16)sValue.toUShort(),bIsBigEndian);
        }
    }
    else if(ui->radioButtonInt->isChecked())
    {
        if(XBinary::checkString_int32(sValue))
        {
            sHex=XBinary::valueToHex((qint32)sValue.toInt(),bIsBigEndian);
        }
    }
    else if(ui->radioButtonUint->isChecked())
    {
        if(XBinary::checkString_uint32(sValue))
        {
            sHex=XBinary::valueToHex((quint32)sValue.toUInt(),bIsBigEndian);
        }
    }
    else if(ui->radioButtonInt64->isChecked())
    {
        if(XBinary::checkString_int64(sValue))
        {
            sHex=XBinary::valueToHex((qint64)sValue.toLongLong(),bIsBigEndian);
        }
    }
    else if(ui->radioButtonUint64->isChecked())
    {
        if(XBinary::checkString_uint64(sValue))
        {
            sHex=XBinary::valueToHex((quint64)sValue.toULongLong(),bIsBigEndian);
        }
    }
    else if(ui->radioButtonFloat->isChecked())
    {
        if(XBinary::checkString_float(sValue))
        {
            sHex=XBinary::valueToHex((float)sValue.toFloat(),bIsBigEndian);
        }
    }
    else if(ui->radioButtonDouble->isChecked())
    {
        if(XBinary::checkString_double(sValue))
        {
            sHex=XBinary::valueToHex((double)sValue.toDouble(),bIsBigEndian);
        }
    }

    ui->lineEditHex->setText(sHex);

    checkValid();
}

void DialogSearch::checkValid()
{
    bool bIsValid=false;

    if(ui->tabWidgetSearch->currentIndex()==0) // Strings
    {
        bIsValid=!(ui->plainTextEditString->toPlainText().isEmpty());
    }
    else if(ui->tabWidgetSearch->currentIndex()==1) // Signature
    {
        bIsValid=!(ui->plainTextEditSignature->toPlainText().isEmpty());
    }
    else if(ui->tabWidgetSearch->currentIndex()==2) // Value
    {
        bIsValid=!(ui->lineEditHex->text().isEmpty());
    }

    ui->pushButtonOK->setEnabled(bIsValid);
}

void DialogSearch::on_plainTextEditString_textChanged()
{
    checkValid();
}

void DialogSearch::on_plainTextEditSignature_textChanged()
{
    checkValid();
}
