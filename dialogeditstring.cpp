/* Copyright (c) 2017-2026 hors<horsicq@gmail.com>
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

#include <QDialogButtonBox>
#include <QPushButton>

#include "ui_dialogeditstring.h"

namespace {

bool setValidationError(QString *pError, const QString &sError)
{
    if (pError) {
        *pError = sError;
    }

    return false;
}

}  // namespace

DialogEditString::DialogEditString(QWidget *pParent, QIODevice *pDevice, DATA_STRUCT *pData_struct)
    : XShortcutsDialog(pParent, false), ui(new Ui::DialogEditString), m_pDevice(pDevice), m_pData_struct(pData_struct), m_initialData(), m_nOriginalSize(0)
{
    if (pData_struct) {
        m_initialData = *pData_struct;
        m_nOriginalSize = (pData_struct->nMaxSize == 0) ? pData_struct->nSize : pData_struct->nMaxSize;
    }

    ui->setupUi(this);

    const bool bBlocked1 = ui->comboBoxType->blockSignals(true);
    const bool bBlocked2 = ui->lineEditString->blockSignals(true);
    const bool bBlocked3 = ui->checkBoxKeepSize->blockSignals(true);
    const bool bBlocked4 = ui->checkBoxNullTerminated->blockSignals(true);

    ui->comboBoxType->addItem(tr("ANSI / Latin-1"), static_cast<quint32>(XBinary::VT_A));
    ui->comboBoxType->addItem(tr("UTF-16 LE"), static_cast<quint32>(XBinary::VT_U));
    ui->comboBoxType->addItem(tr("UTF-8"), static_cast<quint32>(XBinary::VT_UTF8));
    ui->comboBoxType->setCurrentIndex(ui->comboBoxType->findData(static_cast<quint32>(m_initialData.valueType)));
    ui->lineEditString->setText(m_initialData.sString);
    ui->checkBoxKeepSize->setChecked(true);
    ui->checkBoxNullTerminated->setChecked(m_initialData.bIsNullTerminated);

    ui->comboBoxType->blockSignals(bBlocked1);
    ui->lineEditString->blockSignals(bBlocked2);
    ui->checkBoxKeepSize->blockSignals(bBlocked3);
    ui->checkBoxNullTerminated->blockSignals(bBlocked4);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DialogEditString::acceptChanges);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &DialogEditString::reject);

    if (QPushButton *pApplyButton = ui->buttonBox->button(QDialogButtonBox::Ok)) {
        pApplyButton->setText(tr("&Apply"));
        pApplyButton->setDefault(true);
        pApplyButton->setAutoDefault(true);
    }

    updateState();
    adjustView();

    QString sConfigurationError;
    if (configurationValid(&sConfigurationError)) {
        ui->lineEditString->setFocus(Qt::OtherFocusReason);
        ui->lineEditString->selectAll();
    } else if (QPushButton *pCancelButton = ui->buttonBox->button(QDialogButtonBox::Cancel)) {
        pCancelButton->setFocus(Qt::OtherFocusReason);
    }
}

DialogEditString::~DialogEditString()
{
    delete ui;
}

void DialogEditString::adjustView()
{
    setMinimumWidth(qMax(480, minimumSizeHint().width()));
    adjustSize();
}

void DialogEditString::on_comboBoxType_currentIndexChanged(int nIndex)
{
    Q_UNUSED(nIndex)

    updateState();
}

void DialogEditString::on_checkBoxKeepSize_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)

    updateState();
}

void DialogEditString::on_lineEditString_textChanged(const QString &sStrings)
{
    Q_UNUSED(sStrings)

    updateState();
}

void DialogEditString::on_checkBoxNullTerminated_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)

    updateState();
}

bool DialogEditString::configurationValid(QString *pError) const
{
    if (!m_pData_struct) {
        return setValidationError(pError, tr("String information is unavailable."));
    }
    if (!m_pDevice) {
        return setValidationError(pError, tr("The target device is unavailable."));
    }
    if (!m_pDevice->isOpen() || !m_pDevice->isWritable()) {
        return setValidationError(pError, tr("The target device is not open for writing."));
    }
    if (m_pDevice->isSequential()) {
        return setValidationError(pError, tr("Sequential devices cannot be edited by offset."));
    }

    const qint64 nDeviceSize = m_pDevice->size();
    if (nDeviceSize < 0) {
        return setValidationError(pError, tr("The target device size is unavailable."));
    }
    if ((m_initialData.nOffset < 0) || (m_initialData.nOffset > nDeviceSize)) {
        return setValidationError(pError, tr("The string offset is outside the target device."));
    }
    if ((m_nOriginalSize < 0) || (m_nOriginalSize > (nDeviceSize - m_initialData.nOffset))) {
        return setValidationError(pError, tr("The original string span is outside the target device."));
    }
    if ((m_initialData.nSize < 0) || (m_initialData.nSize > m_nOriginalSize)) {
        return setValidationError(pError, tr("The original string size exceeds its writable span."));
    }

    return true;
}

bool DialogEditString::buildCandidate(DATA_STRUCT *pData, QByteArray *pEncodedData, qint64 *pMaximumSize, QString *pError) const
{
    QString sError;
    if (!configurationValid(&sError)) {
        if (pError) {
            *pError = sError;
        }
        return false;
    }

    if (ui->comboBoxType->currentIndex() < 0) {
        if (pError) {
            *pError = tr("Select a supported text encoding.");
        }
        return false;
    }

    const XBinary::VT valueType = static_cast<XBinary::VT>(ui->comboBoxType->currentData().toUInt());
    if (!isSupportedType(valueType)) {
        if (pError) {
            *pError = tr("Select a supported text encoding.");
        }
        return false;
    }

    const QString sString = ui->lineEditString->text();
    const QByteArray baContent = XBinary::getStringData(valueType, sString, false);
    const QByteArray baEncoded = XBinary::getStringData(valueType, sString, ui->checkBoxNullTerminated->isChecked());
    const qint64 nMaximumSize = maximumEncodedSize();

    if (baEncoded.isEmpty()) {
        if (pError) {
            *pError = tr("Enter text or enable null termination.");
        }
        return false;
    }
    if ((valueType == XBinary::VT_A) && (QString::fromLatin1(baContent) != sString)) {
        if (pError) {
            *pError = tr("ANSI / Latin-1 cannot represent every character in the text.");
        }
        return false;
    }
    if (baEncoded.size() > nMaximumSize) {
        if (pError) {
            *pError = tr("Encoded value needs %1 bytes; maximum is %2.").arg(baEncoded.size()).arg(nMaximumSize);
        }
        return false;
    }

    if (pData) {
        *pData = m_initialData;
        pData->valueType = valueType;
        pData->sString = sString;
        pData->nSize = baContent.size();
        pData->nMaxSize = nMaximumSize;
        pData->bIsNullTerminated = ui->checkBoxNullTerminated->isChecked();
    }
    if (pEncodedData) {
        *pEncodedData = baEncoded;
    }
    if (pMaximumSize) {
        *pMaximumSize = nMaximumSize;
    }

    return true;
}

qint64 DialogEditString::maximumEncodedSize() const
{
    if (!m_pDevice) {
        return 0;
    }

    if (ui->checkBoxKeepSize->isChecked()) {
        return m_nOriginalSize;
    }

    return qMin<qint64>(0x100, m_pDevice->size() - m_initialData.nOffset);
}

bool DialogEditString::isSupportedType(XBinary::VT valueType)
{
    return (valueType == XBinary::VT_A) || (valueType == XBinary::VT_U) || (valueType == XBinary::VT_UTF8);
}

void DialogEditString::updateState()
{
    DATA_STRUCT candidate = {};
    QByteArray baEncoded;
    qint64 nMaximumSize = 0;
    QString sError;
    const bool bValid = buildCandidate(&candidate, &baEncoded, &nMaximumSize, &sError);
    QString sConfigurationError;
    const bool bConfigurationValid = configurationValid(&sConfigurationError);

    ui->comboBoxType->setEnabled(bConfigurationValid);
    ui->lineEditString->setEnabled(bConfigurationValid);
    ui->checkBoxKeepSize->setEnabled(bConfigurationValid);
    ui->checkBoxNullTerminated->setEnabled(bConfigurationValid);

    if (QPushButton *pApplyButton = ui->buttonBox->button(QDialogButtonBox::Ok)) {
        pApplyButton->setEnabled(bValid);
    }

    const QString sStatus = bValid ? tr("Uses %1 of %2 bytes (%3 available).").arg(baEncoded.size()).arg(nMaximumSize).arg(nMaximumSize - baEncoded.size()) : sError;
    ui->labelAvailable->setText(sStatus);
    ui->lineEditString->setAccessibleDescription(tr("Text is encoded before it is written. %1").arg(sStatus));
}

void DialogEditString::acceptChanges()
{
    DATA_STRUCT candidate = {};
    QString sError;
    if (!buildCandidate(&candidate, nullptr, nullptr, &sError)) {
        ui->labelAvailable->setText(sError);
        ui->lineEditString->setFocus(Qt::OtherFocusReason);
        ui->lineEditString->selectAll();
        return;
    }

    *m_pData_struct = candidate;
    accept();
}

void DialogEditString::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
}
