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
#include "dialogsearch.h"

#include "ui_dialogsearch.h"

DialogSearch::DialogSearch(QWidget *pParent, QIODevice *pDevice, XBinary::SEARCHDATA *pSearchData, SEARCHMODE searchMode, OPTIONS options)
    : QDialog(pParent), ui(new Ui::DialogSearch)
{
    ui->setupUi(this);

    const bool bBlocked1 = ui->comboBoxEndianness->blockSignals(true);
    const bool bBlocked2 = ui->lineEditValue->blockSignals(true);
    const bool bBlocked3 = ui->tabWidgetSearch->blockSignals(true);
    const bool bBlocked4 = ui->radioButtonChar->blockSignals(true);
    const bool bBlocked5 = ui->radioButtonDouble->blockSignals(true);
    const bool bBlocked6 = ui->radioButtonFloat->blockSignals(true);
    const bool bBlocked7 = ui->radioButtonInt->blockSignals(true);
    const bool bBlocked8 = ui->radioButtonInt64->blockSignals(true);
    const bool bBlocked9 = ui->radioButtonShort->blockSignals(true);
    const bool bBlocked10 = ui->radioButtonUchar->blockSignals(true);
    const bool bBlocked11 = ui->radioButtonUint->blockSignals(true);
    const bool bBlocked12 = ui->radioButtonUint64->blockSignals(true);
    const bool bBlocked13 = ui->radioButtonUshort->blockSignals(true);

    this->g_pDevice = pDevice;
    this->g_pSearchData = pSearchData;

    ui->comboBoxSearchFrom->addItem(tr("Begin"));
    ui->comboBoxSearchFrom->addItem(tr("Cursor"));

    ui->comboBoxType->addItem(QString("ANSI"));
    ui->comboBoxType->addItem(QString("Unicode"));
    ui->comboBoxType->addItem(QString("UTF8"));

    ui->comboBoxEndianness->addItem(QString("LE"));
    ui->comboBoxEndianness->addItem(QString("BE"));

    ui->plainTextEditString->setFocus();

    ui->tabWidgetSearch->setCurrentIndex(0);

    ui->lineEditValue->setText("0");

    ui->radioButtonDword->setChecked(true);

    ajustValue();

    qint32 nCurrentTab = 0;

    if (searchMode == SEARCHMODE_STRING) {
        nCurrentTab = 0;
    } else if (searchMode == SEARCHMODE_SIGNATURE) {
        nCurrentTab = 1;
    } else if (searchMode == SEARCHMODE_VALUE) {
        nCurrentTab = 2;
    }

    ui->tabWidgetSearch->setCurrentIndex(nCurrentTab);

    ui->comboBoxEndianness->blockSignals(bBlocked1);
    ui->lineEditValue->blockSignals(bBlocked2);
    ui->tabWidgetSearch->blockSignals(bBlocked3);
    ui->radioButtonChar->blockSignals(bBlocked4);
    ui->radioButtonDouble->blockSignals(bBlocked5);
    ui->radioButtonFloat->blockSignals(bBlocked6);
    ui->radioButtonInt->blockSignals(bBlocked7);
    ui->radioButtonInt64->blockSignals(bBlocked8);
    ui->radioButtonShort->blockSignals(bBlocked9);
    ui->radioButtonUchar->blockSignals(bBlocked10);
    ui->radioButtonUint->blockSignals(bBlocked11);
    ui->radioButtonUint64->blockSignals(bBlocked12);
    ui->radioButtonUshort->blockSignals(bBlocked13);

    if (options.bShowBegin) {
        ui->groupBoxSearchFrom->show();
    } else {
        ui->groupBoxSearchFrom->hide();
    }
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
    if (ui->comboBoxSearchFrom->currentIndex() == 0) {
        g_pSearchData->startFrom = XBinary::SF_BEGIN;
    } else {
        g_pSearchData->startFrom = XBinary::SF_CURRENTOFFSET;
    }

    if (ui->tabWidgetSearch->currentIndex() == SEARCHMODE_STRING)  // Strings
    {
        bool bMatchCase = ui->checkBoxMatchCase->isChecked();

        if (ui->comboBoxType->currentIndex() == 0)  // ANSI
        {
            if (bMatchCase) {
                g_pSearchData->valueType = XBinary::VT_ANSISTRING;
            } else {
                g_pSearchData->valueType = XBinary::VT_ANSISTRING_I;
            }
        } else if (ui->comboBoxType->currentIndex() == 1)  // UNICODE
        {
            if (bMatchCase) {
                g_pSearchData->valueType = XBinary::VT_UNICODESTRING;
            } else {
                g_pSearchData->valueType = XBinary::VT_UNICODESTRING_I;
            }
        } else if (ui->comboBoxType->currentIndex() == 2)  // UTF8
        {
            if (bMatchCase) {
                g_pSearchData->valueType = XBinary::VT_UTF8STRING;
            } else {
                g_pSearchData->valueType = XBinary::VT_UTF8STRING_I;
            }
        }

        QString sText = ui->plainTextEditString->toPlainText();

        if (sText.length() > 256)  // TODO const
        {
            sText.resize(256);
        }

        g_pSearchData->varValue = sText;
    } else if (ui->tabWidgetSearch->currentIndex() == SEARCHMODE_SIGNATURE)  // Signature
    {
        QString sText = ui->plainTextEditSignature->toPlainText();

        if (sText.length() > 256)  // TODO const
        {
            sText.resize(256);
        }

        g_pSearchData->valueType = XBinary::VT_SIGNATURE;
        g_pSearchData->varValue = sText;                                 // TODO Check
    } else if (ui->tabWidgetSearch->currentIndex() == SEARCHMODE_VALUE)  // Value
    {
        g_pSearchData->bIsBigEndian = (ui->comboBoxEndianness->currentIndex() == 1);

        g_pSearchData->varValue = ui->lineEditValue->text();

        if (ui->radioButtonByte->isChecked()) {
            g_pSearchData->valueType = XBinary::VT_BYTE;
        } else if (ui->radioButtonWord->isChecked()) {
            g_pSearchData->valueType = XBinary::VT_WORD;
        } else if (ui->radioButtonDword->isChecked()) {
            g_pSearchData->valueType = XBinary::VT_DWORD;
        } else if (ui->radioButtonQword->isChecked()) {
            g_pSearchData->valueType = XBinary::VT_QWORD;
        } else if (ui->radioButtonChar->isChecked()) {
            g_pSearchData->valueType = XBinary::VT_CHAR;
        } else if (ui->radioButtonUchar->isChecked()) {
            g_pSearchData->valueType = XBinary::VT_UCHAR;
        } else if (ui->radioButtonDouble->isChecked()) {
            g_pSearchData->valueType = XBinary::VT_DOUBLE;
        } else if (ui->radioButtonFloat->isChecked()) {
            g_pSearchData->valueType = XBinary::VT_FLOAT;
        } else if (ui->radioButtonInt->isChecked()) {
            g_pSearchData->valueType = XBinary::VT_INT;
        } else if (ui->radioButtonInt64->isChecked()) {
            g_pSearchData->valueType = XBinary::VT_INT64;
        } else if (ui->radioButtonShort->isChecked()) {
            g_pSearchData->valueType = XBinary::VT_SHORT;
        } else if (ui->radioButtonUchar->isChecked()) {
            g_pSearchData->valueType = XBinary::VT_UCHAR;
        } else if (ui->radioButtonUint->isChecked()) {
            g_pSearchData->valueType = XBinary::VT_UINT;
        } else if (ui->radioButtonUint64->isChecked()) {
            g_pSearchData->valueType = XBinary::VT_UINT64;
        } else if (ui->radioButtonUshort->isChecked()) {
            g_pSearchData->valueType = XBinary::VT_USHORT;
        }
    }

    QDialog::accept();
}

void DialogSearch::on_tabWidgetSearch_currentChanged(int nIndex)
{
    if (nIndex == SEARCHMODE_STRING) {
        ui->plainTextEditString->setFocus();
    } else if (nIndex == SEARCHMODE_SIGNATURE) {
        ui->plainTextEditSignature->setFocus();
    } else if (nIndex == SEARCHMODE_VALUE) {
        ui->lineEditValue->setFocus();
    }
}

void DialogSearch::on_lineEditValue_textChanged(const QString &sText)
{
    Q_UNUSED(sText)

    ajustValue();
}

void DialogSearch::on_comboBoxEndianness_currentIndexChanged(int nIndex)
{
    Q_UNUSED(nIndex)

    ajustValue();
}

void DialogSearch::on_radioButtonByte_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)

    ajustValue();
}

void DialogSearch::on_radioButtonWord_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)

    ajustValue();
}

void DialogSearch::on_radioButtonDword_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)

    ajustValue();
}

void DialogSearch::on_radioButtonQword_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)

    ajustValue();
}

void DialogSearch::on_radioButtonChar_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)

    ajustValue();
}

void DialogSearch::on_radioButtonUchar_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)

    ajustValue();
}

void DialogSearch::on_radioButtonShort_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)

    ajustValue();
}

void DialogSearch::on_radioButtonUshort_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)

    ajustValue();
}

void DialogSearch::on_radioButtonInt_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)

    ajustValue();
}

void DialogSearch::on_radioButtonUint_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)

    ajustValue();
}

void DialogSearch::on_radioButtonInt64_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)

    ajustValue();
}

void DialogSearch::on_radioButtonUint64_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)

    ajustValue();
}

void DialogSearch::on_radioButtonFloat_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)

    ajustValue();
}

void DialogSearch::on_radioButtonDouble_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)

    ajustValue();
}

void DialogSearch::ajustValue()
{
    QString sValue = ui->lineEditValue->text();
    QString sHex;

    bool bIsBigEndian = (ui->comboBoxEndianness->currentIndex() == 1);

    if (ui->radioButtonByte->isChecked()) {
        if (XBinary::checkString_byte(sValue)) {
            sHex = XBinary::valueToHex((quint8)sValue.toUShort(nullptr, 16));
        }
    } else if (ui->radioButtonWord->isChecked()) {
        if (XBinary::checkString_word(sValue)) {
            sHex = XBinary::valueToHex((quint16)sValue.toUShort(nullptr, 16), bIsBigEndian);
        }
    } else if (ui->radioButtonDword->isChecked()) {
        if (XBinary::checkString_dword(sValue)) {
            sHex = XBinary::valueToHex((qint32)sValue.toUInt(nullptr, 16), bIsBigEndian);
        }
    } else if (ui->radioButtonQword->isChecked()) {
        if (XBinary::checkString_qword(sValue)) {
            sHex = XBinary::valueToHex((quint64)sValue.toULongLong(nullptr, 16), bIsBigEndian);
        }
    } else if (ui->radioButtonChar->isChecked()) {
        if (XBinary::checkString_int8(sValue)) {
            sHex = XBinary::valueToHex((qint8)sValue.toShort(), bIsBigEndian);
        }
    } else if (ui->radioButtonUchar->isChecked()) {
        if (XBinary::checkString_uint8(sValue)) {
            sHex = XBinary::valueToHex((quint8)sValue.toUShort(), bIsBigEndian);
        }
    } else if (ui->radioButtonShort->isChecked()) {
        if (XBinary::checkString_int16(sValue)) {
            sHex = XBinary::valueToHex((qint16)sValue.toShort(), bIsBigEndian);
        }
    } else if (ui->radioButtonUshort->isChecked()) {
        if (XBinary::checkString_uint16(sValue)) {
            sHex = XBinary::valueToHex((quint16)sValue.toUShort(), bIsBigEndian);
        }
    } else if (ui->radioButtonInt->isChecked()) {
        if (XBinary::checkString_int32(sValue)) {
            sHex = XBinary::valueToHex((qint32)sValue.toInt(), bIsBigEndian);
        }
    } else if (ui->radioButtonUint->isChecked()) {
        if (XBinary::checkString_uint32(sValue)) {
            sHex = XBinary::valueToHex((quint32)sValue.toUInt(), bIsBigEndian);
        }
    } else if (ui->radioButtonInt64->isChecked()) {
        if (XBinary::checkString_int64(sValue)) {
            sHex = XBinary::valueToHex((qint64)sValue.toLongLong(), bIsBigEndian);
        }
    } else if (ui->radioButtonUint64->isChecked()) {
        if (XBinary::checkString_uint64(sValue)) {
            sHex = XBinary::valueToHex((quint64)sValue.toULongLong(), bIsBigEndian);
        }
    } else if (ui->radioButtonFloat->isChecked()) {
        if (XBinary::checkString_float(sValue)) {
            sHex = XBinary::valueToHex((float)sValue.toFloat(), bIsBigEndian);
        }
    } else if (ui->radioButtonDouble->isChecked()) {
        if (XBinary::checkString_double(sValue)) {
            sHex = XBinary::valueToHex((double)sValue.toDouble(), bIsBigEndian);
        }
    }

    ui->lineEditHex->setText(sHex);

    checkValid();
}

void DialogSearch::checkValid()
{
    bool bIsValid = false;

    if (ui->tabWidgetSearch->currentIndex() == SEARCHMODE_STRING)  // Strings
    {
        bIsValid = !(ui->plainTextEditString->toPlainText().isEmpty());
    } else if (ui->tabWidgetSearch->currentIndex() == SEARCHMODE_SIGNATURE)  // Signature
    {
        bIsValid = !(ui->plainTextEditSignature->toPlainText().isEmpty());
    } else if (ui->tabWidgetSearch->currentIndex() == SEARCHMODE_VALUE)  // Value
    {
        bIsValid = !(ui->lineEditHex->text().isEmpty());
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
