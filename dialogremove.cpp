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
#include "dialogremove.h"

#include <QDialogButtonBox>
#include <QLineEdit>
#include <QPushButton>

#include "ui_dialogremove.h"

DialogRemove::DialogRemove(QWidget *pParent, DATA *pData)
    : XShortcutsDialog(pParent, false), ui(new Ui::DialogRemove), m_pData(pData), m_pRemoveButton(nullptr)
{
    ui->setupUi(this);

    m_pRemoveButton = ui->buttonBox->addButton(tr("&Remove"), QDialogButtonBox::DestructiveRole);
    m_pRemoveButton->setObjectName(QStringLiteral("pushButtonRemove"));
    m_pRemoveButton->setAutoDefault(false);
    m_pRemoveButton->setDefault(false);

    connect(m_pRemoveButton, &QPushButton::clicked, this, &DialogRemove::acceptRemove);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    ui->checkBoxHex->setChecked(true);
    ui->lineEditOffset->setValue_int64(m_pData ? m_pData->nOffset : 0);
    ui->lineEditSize->setValue_int64(m_pData ? m_pData->nSize : 0);

    connect(ui->lineEditOffset, &QLineEdit::textChanged, this, &DialogRemove::updateState);
    connect(ui->lineEditSize, &QLineEdit::textChanged, this, &DialogRemove::updateState);

    adjustView();
    updateState();
    ui->lineEditOffset->setFocus(Qt::OtherFocusReason);
    ui->lineEditOffset->selectAll();
}

DialogRemove::~DialogRemove()
{
    delete ui;
}

void DialogRemove::adjustView()
{
    setMinimumWidth(440);
}

bool DialogRemove::readRange(qint64 *pnOffset, qint64 *pnSize, QString *psError) const
{
    const auto fail = [&](const QString &sError) {
        if (psError) {
            *psError = sError;
        }
        return false;
    };

    if (!m_pData || (m_pData->nMaxSize < 0)) {
        return fail(tr("The data range is unavailable."));
    }
    if (m_pData->nMaxSize == 0) {
        return fail(tr("There are no bytes to remove."));
    }
    if (ui->lineEditOffset->text().trimmed().isEmpty()) {
        return fail(tr("Enter the offset where removal should begin."));
    }
    if (ui->lineEditSize->text().trimmed().isEmpty()) {
        return fail(tr("Enter the number of bytes to remove."));
    }

    const qint64 nOffset = ui->lineEditOffset->getValue_int64();
    const qint64 nSize = ui->lineEditSize->getValue_int64();

    if (nOffset < 0) {
        return fail(tr("The offset cannot be negative."));
    }
    if (nSize <= 0) {
        return fail(tr("The number of bytes must be greater than zero."));
    }
    if (nOffset >= m_pData->nMaxSize) {
        return fail(tr("The offset must be before the end of the data."));
    }

    const qint64 nAvailable = m_pData->nMaxSize - nOffset;
    if (nSize > nAvailable) {
        return fail(tr("The range extends past the end of the data. At most %1 bytes can be removed from this offset.").arg(nAvailable));
    }

    if (pnOffset) {
        *pnOffset = nOffset;
    }
    if (pnSize) {
        *pnSize = nSize;
    }
    if (psError) {
        psError->clear();
    }

    return true;
}

QString DialogRemove::formatOffset(qint64 nValue) const
{
    if (ui->checkBoxHex->isChecked()) {
        return QStringLiteral("0x") + QString::number(nValue, 16).toUpper();
    }

    return QString::number(nValue);
}

void DialogRemove::acceptRemove()
{
    qint64 nOffset = 0;
    qint64 nSize = 0;
    QString sError;

    if (!readRange(&nOffset, &nSize, &sError)) {
        updateState();
        return;
    }

    m_pData->nOffset = nOffset;
    m_pData->nSize = nSize;
    accept();
}

void DialogRemove::updateState()
{
    const qint64 nMaxSize = m_pData ? m_pData->nMaxSize : -1;
    if (nMaxSize >= 0) {
        ui->labelDataSizeValue->setText(tr("%1 bytes (0x%2)").arg(nMaxSize).arg(QString::number(nMaxSize, 16).toUpper()));
    } else {
        ui->labelDataSizeValue->setText(tr("Unavailable"));
    }

    qint64 nOffset = 0;
    qint64 nSize = 0;
    QString sError;
    const bool bValid = readRange(&nOffset, &nSize, &sError);

    m_pRemoveButton->setEnabled(bValid);
    if (bValid) {
        const qint64 nLastOffset = nOffset + nSize - 1;
        const qint64 nNewSize = nMaxSize - nSize;
        ui->labelStatus->setText(tr("Remove %1 bytes at offsets %2–%3. New data size: %4 bytes.")
                                     .arg(nSize)
                                     .arg(formatOffset(nOffset), formatOffset(nLastOffset))
                                     .arg(nNewSize));
    } else {
        ui->labelStatus->setText(sError);
    }
}

void DialogRemove::on_checkBoxHex_toggled(bool bChecked)
{
    if (bChecked) {
        ui->lineEditOffset->setMode(XLineEditHEX::_MODE_HEX);
        ui->lineEditSize->setMode(XLineEditHEX::_MODE_HEX);
    } else {
        ui->lineEditOffset->setMode(XLineEditHEX::_MODE_DEC);
        ui->lineEditSize->setMode(XLineEditHEX::_MODE_DEC);
    }

    if (m_pRemoveButton) {
        updateState();
    }
}

void DialogRemove::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
}
