/* Copyright (c) 2017-2025 hors<horsicq@gmail.com>
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
#include "dialogeditstring.h"

#include "ui_dialogeditstring.h"

DialogEditString::DialogEditString(QWidget *pParent, QIODevice *pDevice, DATA_STRUCT *pData_struct) : XShortcutsDialog(pParent, false), ui(new Ui::DialogEditString)
{
    ui->setupUi(this);

    this->g_pDevice = pDevice;
    this->g_pData_struct = pData_struct;

    g_nSize = pData_struct->nSize;

    const bool bBlocked1 = ui->comboBoxType->blockSignals(true);
    const bool bBlocked2 = ui->lineEditString->blockSignals(true);
    const bool bBlocked3 = ui->checkBoxKeepSize->blockSignals(true);
    const bool bBlocked4 = ui->checkBoxNullTerminated->blockSignals(true);

    ui->comboBoxType->addItem(QString("ANSI"), XBinary::VT_A);
    ui->comboBoxType->addItem(QString("Unicode"), XBinary::VT_U);
    ui->comboBoxType->addItem(QString("UTF8"), XBinary::VT_UTF8);

    qint32 nNumberOfRecords = ui->comboBoxType->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (ui->comboBoxType->itemData(i) == pData_struct->valueType) {
            ui->comboBoxType->setCurrentIndex(i);

            break;
        }
    }

    ui->lineEditString->setText(pData_struct->sString);
    ui->checkBoxKeepSize->setChecked(true);
    ui->checkBoxNullTerminated->setChecked(pData_struct->bIsNullTerminated);

    ui->comboBoxType->blockSignals(bBlocked1);
    ui->lineEditString->blockSignals(bBlocked2);
    ui->checkBoxKeepSize->blockSignals(bBlocked3);
    ui->checkBoxNullTerminated->blockSignals(bBlocked4);

    adjust();
}

DialogEditString::~DialogEditString()
{
    delete ui;
}

void DialogEditString::adjustView()
{
}

void DialogEditString::on_pushButtonCancel_clicked()
{
    this->close();
}

void DialogEditString::on_pushButtonOK_clicked()
{
    accept();
}

void DialogEditString::on_comboBoxType_currentIndexChanged(int nIndex)
{
    Q_UNUSED(nIndex)

    adjust();
}

void DialogEditString::on_checkBoxKeepSize_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)

    adjust();
}

void DialogEditString::on_lineEditString_textChanged(const QString &sStrings)
{
    Q_UNUSED(sStrings)

    adjust();
}

void DialogEditString::on_checkBoxNullTerminated_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)

    adjust();
}

void DialogEditString::adjust()
{
    qint32 nMax = g_nSize;

    if (ui->checkBoxNullTerminated->isChecked()) {
        if (ui->comboBoxType->currentData().toUInt() == XBinary::VT_U) {
            nMax -= 2;
        } else {
            nMax--;
        }
    }

    if (!ui->checkBoxKeepSize->isChecked()) {
        nMax = qMin((qint64)0x100, g_pDevice->size() - (g_pData_struct->nOffset));
    }

    if (ui->comboBoxType->currentData().toUInt() == XBinary::VT_U) {
        ui->lineEditString->setMaxLength(nMax / 2);
    } else {
        ui->lineEditString->setMaxLength(nMax);
    }

    QByteArray baString =
        XBinary::getStringData((XBinary::VT)(ui->comboBoxType->currentData().toUInt()), ui->lineEditString->text(), ui->checkBoxNullTerminated->isChecked());

    QString sStatus = QString("%1: %2").arg(tr("Bytes available"), QString::number(nMax - baString.size()));

    ui->labelAvailable->setText(sStatus);

    g_pData_struct->valueType = (XBinary::VT)(ui->comboBoxType->currentData().toUInt());
    g_pData_struct->sString = ui->lineEditString->text();
    g_pData_struct->nSize = baString.size();

    if (ui->checkBoxNullTerminated->isChecked()) {
        if (ui->comboBoxType->currentData().toUInt() == XBinary::VT_U) {
            g_pData_struct->nSize -= 2;
        } else {
            g_pData_struct->nSize--;
        }
    }

    g_pData_struct->bIsNullTerminated = ui->checkBoxNullTerminated->isChecked();
}

void DialogEditString::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
}
