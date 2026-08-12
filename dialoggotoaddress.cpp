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
#include "dialoggotoaddress.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <limits>

#include "ui_dialoggotoaddress.h"
#include "xoptions.h"

DialogGoToAddress::DialogGoToAddress(QWidget *pParent, XBinary::_MEMORY_MAP *pMemoryMap, TYPE type, XADDR nCurrentValue)
    : XShortcutsDialog(pParent, false),
      ui(new Ui::DialogGoToAddress),
      m_type(type),
      m_memoryMap(),
      m_bUseMemoryMap(true),
      m_bHasMemoryMap(pMemoryMap != nullptr),
      m_nMinValue(0),
      m_nMaxValue(0),
      m_nValue(nCurrentValue)
{
    if (pMemoryMap) {
        m_memoryMap = *pMemoryMap;
    }
    ui->setupUi(this);
    initialize();
}

DialogGoToAddress::DialogGoToAddress(QWidget *pParent, XADDR nMinValue, XADDR nMaxValue, TYPE type, XADDR nCurrentValue)
    : XShortcutsDialog(pParent, false),
      ui(new Ui::DialogGoToAddress),
      m_type(type),
      m_memoryMap(),
      m_bUseMemoryMap(false),
      m_bHasMemoryMap(false),
      m_nMinValue(nMinValue),
      m_nMaxValue(nMaxValue),
      m_nValue(nCurrentValue)
{
    ui->setupUi(this);
    initialize();
}

DialogGoToAddress::~DialogGoToAddress()
{
    delete ui;
}

void DialogGoToAddress::initialize()
{
    adjustTitle(m_type);

    ui->checkBoxHex->blockSignals(true);
    ui->checkBoxHex->setChecked(true);
    ui->checkBoxHex->blockSignals(false);

    // The allowed target range, rather than the current value, determines the
    // required width. Always use the unsigned 64-bit editor so a dialog opened
    // near zero can still navigate beyond 4 GiB.
    ui->lineEditValue->setValue_uint64(m_nValue, XLineEditHEX::_MODE_HEX);
    XOptions::setMonoFont(ui->lineEditValue);

    connect(ui->lineEditValue, &QLineEdit::textChanged, this, [this](const QString &) { updateState(); });
    connect(ui->lineEditValue, &QLineEdit::returnPressed, this, [this]() {
        QPushButton *pGoButton = ui->buttonBox->button(QDialogButtonBox::Ok);
        if (pGoButton && pGoButton->isEnabled()) {
            acceptValue();
        }
    });
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DialogGoToAddress::acceptValue);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &DialogGoToAddress::reject);

    QPushButton *pGoButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    if (pGoButton) {
        pGoButton->setText(tr("&Go to"));
        pGoButton->setDefault(true);
        pGoButton->setAutoDefault(true);
    }

    updateRangeDescription();
    updateState();
    adjustView();

    const XADDR nSignedMaximum = static_cast<XADDR>((std::numeric_limits<qint64>::max)());
    const bool bSignedDomain = (m_type == TYPE_OFFSET) || (m_type == TYPE_RELVIRTUALADDRESS);
    const bool bConfigurationAvailable = m_bUseMemoryMap ? m_bHasMemoryMap
                                                         : ((m_nMinValue <= m_nMaxValue) && (!bSignedDomain || (m_nMinValue <= nSignedMaximum)));
    ui->lineEditValue->setEnabled(bConfigurationAvailable);
    ui->checkBoxHex->setEnabled(bConfigurationAvailable);

    if (bConfigurationAvailable) {
        ui->lineEditValue->setFocus(Qt::OtherFocusReason);
        ui->lineEditValue->selectAll();
    } else if (QPushButton *pCancelButton = ui->buttonBox->button(QDialogButtonBox::Cancel)) {
        pCancelButton->setFocus(Qt::OtherFocusReason);
    }
}

void DialogGoToAddress::adjustView()
{
    setMinimumWidth(qMax(440, minimumSizeHint().width()));
    adjustSize();
}

XADDR DialogGoToAddress::getValue() const
{
    return m_nValue;
}

void DialogGoToAddress::adjustTitle(DialogGoToAddress::TYPE type)
{
    QString sWindowTitle;
    QString sValueLabel;
    QString sAccessibleName;

    if (type == TYPE_VIRTUALADDRESS) {
        sWindowTitle = tr("Go to Virtual Address");
        sValueLabel = tr("&Virtual address:");
        sAccessibleName = tr("Virtual address");
    } else if (type == TYPE_OFFSET) {
        sWindowTitle = tr("Go to Offset");
        sValueLabel = tr("&Offset:");
        sAccessibleName = tr("Offset");
    } else if (type == TYPE_RELVIRTUALADDRESS) {
        sWindowTitle = tr("Go to Relative Virtual Address");
        sValueLabel = tr("&Relative virtual address:");
        sAccessibleName = tr("Relative virtual address");
    } else {
        sWindowTitle = tr("Go to Address");
        sValueLabel = tr("&Address:");
        sAccessibleName = tr("Address");
    }

    setWindowTitle(sWindowTitle);
    ui->labelValue->setText(sValueLabel);
    ui->lineEditValue->setAccessibleName(sAccessibleName);
}

void DialogGoToAddress::updateRangeDescription()
{
    ui->lineEditValue->setAccessibleDescription(
        ui->checkBoxHex->isChecked() ? tr("Enter unsigned hexadecimal digits without a 0x prefix.") : tr("Enter an unsigned decimal value."));

    if (m_bUseMemoryMap) {
        if (!m_bHasMemoryMap) {
            ui->labelRange->setText(tr("Memory-map information is unavailable."));
            return;
        }
        if (m_type == TYPE_OFFSET) {
            ui->labelRange->setText(tr("Enter a valid file offset backed by mapped data."));
        } else if (m_type == TYPE_RELVIRTUALADDRESS) {
            ui->labelRange->setText(tr("Enter a relative virtual address backed by mapped data."));
        } else {
            ui->labelRange->setText(tr("Enter an address backed by mapped data."));
        }
    } else {
        XADDR nEffectiveMaximum = m_nMaxValue;
        if ((m_type == TYPE_OFFSET) || (m_type == TYPE_RELVIRTUALADDRESS)) {
            nEffectiveMaximum = qMin(nEffectiveMaximum, static_cast<XADDR>((std::numeric_limits<qint64>::max)()));
        }

        if (m_nMinValue <= nEffectiveMaximum) {
            ui->labelRange->setText(tr("Allowed range: %1 to %2 (inclusive).").arg(formatValue(m_nMinValue), formatValue(nEffectiveMaximum)));
        } else {
            ui->labelRange->setText(tr("The allowed range is invalid."));
        }
    }
}

void DialogGoToAddress::updateState()
{
    XADDR nValue = 0;
    QString sError;
    const bool bValid = readValue(&nValue, &sError);
    QPushButton *pGoButton = ui->buttonBox->button(QDialogButtonBox::Ok);

    if (pGoButton) {
        pGoButton->setEnabled(bValid);
    }
    const QString sStatus = bValid ? tr("Ready to go to %1.").arg(formatValue(nValue)) : sError;
    ui->labelStatus->setText(sStatus);
    const QString sBaseDescription = ui->checkBoxHex->isChecked() ? tr("Enter unsigned hexadecimal digits without a 0x prefix.")
                                                                  : tr("Enter an unsigned decimal value.");
    ui->lineEditValue->setAccessibleDescription(sBaseDescription + QLatin1Char(' ') + sStatus);
}

bool DialogGoToAddress::readValue(XADDR *pValue, QString *pError) const
{
    const QString sText = ui->lineEditValue->text().trimmed();
    if (sText.isEmpty()) {
        if (pError) {
            *pError = tr("Enter a destination value.");
        }
        return false;
    }

    bool bSuccess = false;
    const XADDR nValue = sText.toULongLong(&bSuccess, ui->checkBoxHex->isChecked() ? 16 : 10);
    if (!bSuccess) {
        if (pError) {
            *pError = tr("Enter a valid unsigned 64-bit value.");
        }
        return false;
    }

    if (!isValueValid(nValue)) {
        if (pError) {
            if (m_bUseMemoryMap) {
                if (!m_bHasMemoryMap) {
                    *pError = tr("Memory-map information is unavailable.");
                    return false;
                }
                *pError = tr("The value does not identify mapped physical data.");
            } else if (m_nMinValue > m_nMaxValue) {
                *pError = tr("The allowed range is invalid.");
            } else {
                XADDR nEffectiveMaximum = m_nMaxValue;
                if ((m_type == TYPE_OFFSET) || (m_type == TYPE_RELVIRTUALADDRESS)) {
                    nEffectiveMaximum = qMin(nEffectiveMaximum, static_cast<XADDR>((std::numeric_limits<qint64>::max)()));
                }
                *pError = tr("Value must be between %1 and %2 (inclusive).").arg(formatValue(m_nMinValue), formatValue(nEffectiveMaximum));
            }
        }
        return false;
    }

    if (pValue) {
        *pValue = nValue;
    }
    return true;
}

bool DialogGoToAddress::isValueValid(XADDR nValue) const
{
    if (nValue == XADDR_MAX) {
        return false;
    }

    if (!m_bUseMemoryMap) {
        XADDR nEffectiveMaximum = m_nMaxValue;
        if ((m_type == TYPE_OFFSET) || (m_type == TYPE_RELVIRTUALADDRESS)) {
            nEffectiveMaximum = qMin(nEffectiveMaximum, static_cast<XADDR>((std::numeric_limits<qint64>::max)()));
        }

        return (m_nMinValue <= nEffectiveMaximum) && (nValue >= m_nMinValue) && (nValue <= nEffectiveMaximum);
    }
    if (!m_bHasMemoryMap) {
        return false;
    }
    XBinary::_MEMORY_MAP memoryMap = m_memoryMap;

    if (m_type == TYPE_OFFSET) {
        return (nValue <= static_cast<XADDR>((std::numeric_limits<qint64>::max)())) &&
               XBinary::isOffsetValid(&memoryMap, static_cast<qint64>(nValue));
    }
    if (m_type == TYPE_RELVIRTUALADDRESS) {
        return (nValue <= static_cast<XADDR>((std::numeric_limits<qint64>::max)())) &&
               XBinary::isRelAddressValid(&memoryMap, static_cast<qint64>(nValue)) && XBinary::isRelAddressPhysical(&memoryMap, nValue);
    }

    return XBinary::isAddressValid(&memoryMap, nValue) && XBinary::isAddressPhysical(&memoryMap, nValue);
}

QString DialogGoToAddress::formatValue(XADDR nValue) const
{
    if (ui->checkBoxHex->isChecked()) {
        return QStringLiteral("0x") + QString::number(nValue, 16).toUpper();
    }

    return QString::number(nValue);
}

void DialogGoToAddress::acceptValue()
{
    XADDR nValue = 0;
    QString sError;
    if (!readValue(&nValue, &sError)) {
        ui->labelStatus->setText(sError);
        ui->lineEditValue->setFocus(Qt::OtherFocusReason);
        ui->lineEditValue->selectAll();
        return;
    }

    m_nValue = nValue;
    accept();
}

void DialogGoToAddress::on_checkBoxHex_toggled(bool bChecked)
{
    const QString sText = ui->lineEditValue->text().trimmed();
    bool bSuccess = false;
    const XADDR nValue = sText.toULongLong(&bSuccess, bChecked ? 10 : 16);

    if (bSuccess && !sText.isEmpty()) {
        ui->lineEditValue->setValue_uint64(nValue, bChecked ? XLineEditHEX::_MODE_HEX : XLineEditHEX::_MODE_DEC);
    } else {
        ui->lineEditValue->setValue_uint64(0, bChecked ? XLineEditHEX::_MODE_HEX : XLineEditHEX::_MODE_DEC);
        ui->lineEditValue->clear();
    }

    updateRangeDescription();
    updateState();
    ui->lineEditValue->setFocus(Qt::OtherFocusReason);
    ui->lineEditValue->selectAll();
}

void DialogGoToAddress::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
}
