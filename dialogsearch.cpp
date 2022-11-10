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

DialogSearch::DialogSearch(QWidget *pParent, QIODevice *pDevice, SearchProcess::SEARCHDATA *pSearchData, SEARCHMODE searchMode)
    : QDialog(pParent), ui(new Ui::DialogSearch) {
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

    ui->radioButtonUint->setChecked(true);

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
}

DialogSearch::~DialogSearch() {
    delete ui;
}

void DialogSearch::on_pushButtonCancel_clicked() {
    this->close();
}

void DialogSearch::on_pushButtonOK_clicked() {
    if (ui->tabWidgetSearch->currentIndex() == SEARCHMODE_STRING)  // Strings
    {
        bool bMatchCase = ui->checkBoxMatchCase->isChecked();

        if (ui->comboBoxType->currentIndex() == 0)  // ANSI
        {
            if (bMatchCase) {
                g_pSearchData->type = SearchProcess::TYPE_ANSISTRING;
            } else {
                g_pSearchData->type = SearchProcess::TYPE_ANSISTRING_I;
            }
        } else if (ui->comboBoxType->currentIndex() == 1)  // UNICODE
        {
            if (bMatchCase) {
                g_pSearchData->type = SearchProcess::TYPE_UNICODESTRING;
            } else {
                g_pSearchData->type = SearchProcess::TYPE_UNICODESTRING_I;
            }
        } else if (ui->comboBoxType->currentIndex() == 2)  // UTF8
        {
            if (bMatchCase) {
                g_pSearchData->type = SearchProcess::TYPE_UTF8STRING;
            } else {
                g_pSearchData->type = SearchProcess::TYPE_UTF8STRING_I;
            }
        }

        QString sText = ui->plainTextEditString->toPlainText();

        if (sText.length() > 256)  // TODO const
        {
            sText.resize(256);
        }

        g_pSearchData->variant = sText;
        g_pSearchData->nResultSize = sText.size();
    } else if (ui->tabWidgetSearch->currentIndex() == SEARCHMODE_SIGNATURE)  // Signature
    {
        QString sText = ui->plainTextEditSignature->toPlainText();

        if (sText.length() > 256)  // TODO const
        {
            sText.resize(256);
        }

        g_pSearchData->type = SearchProcess::TYPE_SIGNATURE;
        g_pSearchData->variant = sText;
        g_pSearchData->nResultSize = 1;                                  // TODO Check
    } else if (ui->tabWidgetSearch->currentIndex() == SEARCHMODE_VALUE)  // Value
    {
        g_pSearchData->bIsBigEndian = (ui->comboBoxEndianness->currentIndex() == 1);

        g_pSearchData->variant = ui->lineEditValue->text();

        if (ui->radioButtonChar->isChecked()) {
            g_pSearchData->type = SearchProcess::TYPE_VALUE_CHAR;
            g_pSearchData->nResultSize = 1;
        } else if (ui->radioButtonUchar->isChecked()) {
            g_pSearchData->type = SearchProcess::TYPE_VALUE_UCHAR;
            g_pSearchData->nResultSize = 1;
        } else if (ui->radioButtonDouble->isChecked()) {
            g_pSearchData->type = SearchProcess::TYPE_VALUE_DOUBLE;
            g_pSearchData->nResultSize = 8;
        } else if (ui->radioButtonFloat->isChecked()) {
            g_pSearchData->type = SearchProcess::TYPE_VALUE_FLOAT;
            g_pSearchData->nResultSize = 4;
        } else if (ui->radioButtonInt->isChecked()) {
            g_pSearchData->type = SearchProcess::TYPE_VALUE_INT;
            g_pSearchData->nResultSize = 4;
        } else if (ui->radioButtonInt64->isChecked()) {
            g_pSearchData->type = SearchProcess::TYPE_VALUE_INT64;
            g_pSearchData->nResultSize = 8;
        } else if (ui->radioButtonShort->isChecked()) {
            g_pSearchData->type = SearchProcess::TYPE_VALUE_SHORT;
            g_pSearchData->nResultSize = 2;
        } else if (ui->radioButtonUchar->isChecked()) {
            g_pSearchData->type = SearchProcess::TYPE_VALUE_UCHAR;
            g_pSearchData->nResultSize = 1;
        } else if (ui->radioButtonUint->isChecked()) {
            g_pSearchData->type = SearchProcess::TYPE_VALUE_UINT;
            g_pSearchData->nResultSize = 4;
        } else if (ui->radioButtonUint64->isChecked()) {
            g_pSearchData->type = SearchProcess::TYPE_VALUE_UINT64;
            g_pSearchData->nResultSize = 8;
        } else if (ui->radioButtonUshort->isChecked()) {
            g_pSearchData->type = SearchProcess::TYPE_VALUE_USHORT;
            g_pSearchData->nResultSize = 2;
        }
    }

    DialogSearchProcess dsp(this, g_pDevice, g_pSearchData);

    done(dsp.showDialogDelay(1000));
}

void DialogSearch::on_tabWidgetSearch_currentChanged(int nIndex) {
    if (nIndex == SEARCHMODE_STRING) {
        ui->plainTextEditString->setFocus();
    } else if (nIndex == SEARCHMODE_SIGNATURE) {
        ui->plainTextEditSignature->setFocus();
    } else if (nIndex == SEARCHMODE_VALUE) {
        ui->lineEditValue->setFocus();
    }
}

void DialogSearch::on_lineEditValue_textChanged(const QString &sText) {
    Q_UNUSED(sText)

    ajustValue();
}

void DialogSearch::on_comboBoxEndianness_currentIndexChanged(int nIndex) {
    Q_UNUSED(nIndex)

    ajustValue();
}

void DialogSearch::on_radioButtonChar_toggled(bool bChecked) {
    Q_UNUSED(bChecked)

    ajustValue();
}

void DialogSearch::on_radioButtonUchar_toggled(bool bChecked) {
    Q_UNUSED(bChecked)

    ajustValue();
}

void DialogSearch::on_radioButtonShort_toggled(bool bChecked) {
    Q_UNUSED(bChecked)

    ajustValue();
}

void DialogSearch::on_radioButtonUshort_toggled(bool bChecked) {
    Q_UNUSED(bChecked)

    ajustValue();
}

void DialogSearch::on_radioButtonInt_toggled(bool bChecked) {
    Q_UNUSED(bChecked)

    ajustValue();
}

void DialogSearch::on_radioButtonUint_toggled(bool bChecked) {
    Q_UNUSED(bChecked)

    ajustValue();
}

void DialogSearch::on_radioButtonInt64_toggled(bool bChecked) {
    Q_UNUSED(bChecked)

    ajustValue();
}

void DialogSearch::on_radioButtonUint64_toggled(bool bChecked) {
    Q_UNUSED(bChecked)

    ajustValue();
}

void DialogSearch::on_radioButtonFloat_toggled(bool bChecked) {
    Q_UNUSED(bChecked)

    ajustValue();
}

void DialogSearch::on_radioButtonDouble_toggled(bool bChecked) {
    Q_UNUSED(bChecked)

    ajustValue();
}

void DialogSearch::ajustValue() {
    QString sValue = ui->lineEditValue->text();
    QString sHex;

    bool bIsBigEndian = (ui->comboBoxEndianness->currentIndex() == 1);

    if (ui->radioButtonChar->isChecked()) {
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

void DialogSearch::checkValid() {
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

void DialogSearch::on_plainTextEditString_textChanged() {
    checkValid();
}

void DialogSearch::on_plainTextEditSignature_textChanged() {
    checkValid();
}
