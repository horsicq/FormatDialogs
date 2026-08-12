/* Copyright (c) 2019-2026 hors<horsicq@gmail.com>
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
#include "dialogresize.h"

#include "ui_dialogresize.h"

#include <QDialogButtonBox>
#include <QLocale>
#include <QMessageBox>
#include <QPushButton>
#include <limits>

DialogResize::DialogResize(QWidget *pParent, DATA *pData) : XShortcutsDialog(pParent, false), ui(new Ui::DialogResize), m_pData(pData)
{
    ui->setupUi(this);

    ui->checkBoxHex->setChecked(true);
    ui->lineEditValue->setMaxValue((std::numeric_limits<qint64>::max)());
    // Select the 64-bit validator even when the initial value is unavailable.
    ui->lineEditValue->setValue_uint64(0);

    if (m_pData && (m_pData->nNewSize >= 0)) {
        ui->lineEditValue->setValue_uint64((quint64)m_pData->nNewSize);
    } else {
        ui->lineEditValue->clear();
    }

    connect(ui->lineEditValue, &QLineEdit::textChanged, this, &DialogResize::updateState);
    connect(ui->checkBoxHex, &QCheckBox::toggled, this, &DialogResize::setHexMode);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DialogResize::acceptResize);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &DialogResize::reject);

    QPushButton *pOKButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    QPushButton *pCancelButton = ui->buttonBox->button(QDialogButtonBox::Cancel);

    if (pOKButton) {
        pOKButton->setDefault(true);
        pOKButton->setAutoDefault(true);
    }

    setTabOrder(ui->lineEditValue, ui->checkBoxHex);
    if (pOKButton) {
        setTabOrder(ui->checkBoxHex, pOKButton);
        if (pCancelButton) {
            setTabOrder(pOKButton, pCancelButton);
        }
    }

    updateState();
    adjustSize();
    ui->lineEditValue->setFocus(Qt::OtherFocusReason);
    ui->lineEditValue->selectAll();
}

DialogResize::~DialogResize()
{
    delete ui;
}

void DialogResize::adjustView()
{
    adjustSize();
}

void DialogResize::acceptResize()
{
    qint64 nValue = 0;
    QString sError;

    if (!readNewSize(&nValue, &sError)) {
        updateState();
        ui->lineEditValue->setFocus(Qt::OtherFocusReason);
        ui->lineEditValue->selectAll();
        return;
    }

    if (nValue < m_pData->nOldSize) {
        const qint64 nRemoved = m_pData->nOldSize - nValue;
        const QString sMessage =
            tr("Reducing the size from %1 to %2 will permanently remove %3 from the end.\n\nContinue?")
                .arg(formatSize(m_pData->nOldSize), formatSize(nValue), formatSize(nRemoved));

        if (QMessageBox::warning(this, tr("Confirm shrinking"), sMessage, QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel) !=
            QMessageBox::Yes) {
            ui->lineEditValue->setFocus(Qt::OtherFocusReason);
            ui->lineEditValue->selectAll();
            return;
        }
    }

    m_pData->nNewSize = nValue;

    accept();
}

void DialogResize::setHexMode(bool bChecked)
{
    if (bChecked) {
        ui->lineEditValue->setMode(XLineEditHEX::_MODE_HEX);
    } else {
        ui->lineEditValue->setMode(XLineEditHEX::_MODE_DEC);
    }

    updateState();
    ui->lineEditValue->setFocus(Qt::OtherFocusReason);
    ui->lineEditValue->selectAll();
}

void DialogResize::updateState()
{
    QPushButton *pOKButton = ui->buttonBox->button(QDialogButtonBox::Ok);

    if (!m_pData) {
        ui->labelCurrentValue->setText(tr("Unavailable"));
        ui->labelStatus->setText(tr("Resize information is unavailable."));
        ui->lineEditValue->setEnabled(false);
        ui->checkBoxHex->setEnabled(false);
        if (pOKButton) {
            pOKButton->setEnabled(false);
        }
        return;
    }

    if (m_pData->nOldSize < 0) {
        ui->labelCurrentValue->setText(tr("Invalid"));
        ui->labelStatus->setText(tr("The current size is invalid."));
        ui->lineEditValue->setEnabled(false);
        ui->checkBoxHex->setEnabled(false);
        if (pOKButton) {
            pOKButton->setEnabled(false);
        }
        return;
    }

    ui->lineEditValue->setEnabled(true);
    ui->checkBoxHex->setEnabled(true);
    ui->labelCurrentValue->setText(formatInputValue(m_pData->nOldSize));
    ui->labelCurrentValue->setToolTip(formatSize(m_pData->nOldSize));

    qint64 nValue = 0;
    QString sError;
    const bool bValid = readNewSize(&nValue, &sError);

    if (pOKButton) {
        pOKButton->setEnabled(bValid);
    }

    if (!bValid) {
        ui->labelStatus->setText(sError);
    } else if (nValue < m_pData->nOldSize) {
        ui->labelStatus->setText(tr("Warning: shrinking will remove %1 from the end.").arg(formatSize(m_pData->nOldSize - nValue)));
    } else if (nValue > m_pData->nOldSize) {
        ui->labelStatus->setText(tr("The file will grow by %1.").arg(formatSize(nValue - m_pData->nOldSize)));
    } else {
        ui->labelStatus->setText(tr("The size is unchanged."));
    }
}

bool DialogResize::readNewSize(qint64 *pValue, QString *pError) const
{
    if (!m_pData) {
        if (pError) {
            *pError = tr("Resize information is unavailable.");
        }
        return false;
    }

    if (m_pData->nOldSize < 0) {
        if (pError) {
            *pError = tr("The current size is invalid.");
        }
        return false;
    }

    const QString sText = ui->lineEditValue->text().trimmed();

    if (sText.isEmpty()) {
        if (pError) {
            *pError = tr("Enter a new size.");
        }
        return false;
    }

    bool bSuccess = false;
    const quint64 nValue = sText.toULongLong(&bSuccess, ui->checkBoxHex->isChecked() ? 16 : 10);

    if ((!bSuccess) || (nValue > (quint64)(std::numeric_limits<qint64>::max)())) {
        if (pError) {
            *pError = tr("The new size must be between 0 and %1.").arg(formatSize((std::numeric_limits<qint64>::max)()));
        }
        return false;
    }

    if (pValue) {
        *pValue = (qint64)nValue;
    }

    return true;
}

QString DialogResize::formatInputValue(qint64 nValue) const
{
    if (ui->checkBoxHex->isChecked()) {
        return QString("%1").arg((quint64)nValue, 16, 16, QChar('0')).toUpper();
    }

    return QString::number(nValue);
}

QString DialogResize::formatSize(qint64 nValue) const
{
    const QString sDecimal = QLocale().toString(nValue);
    const QString sHex = QString::number((quint64)nValue, 16).toUpper();

    if (ui->checkBoxHex->isChecked()) {
        return tr("0x%1 (%2 B)").arg(sHex, sDecimal);
    }

    return tr("%1 B (0x%2)").arg(sDecimal, sHex);
}

void DialogResize::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
}
