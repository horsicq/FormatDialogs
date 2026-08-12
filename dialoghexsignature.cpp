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
#include "dialoghexsignature.h"

#include <QApplication>
#include <QClipboard>
#include <QDialogButtonBox>
#include <QFontMetrics>
#include <QHeaderView>
#include <QKeyEvent>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QTableWidget>
#include <QTableWidgetItem>

#include "dialogsearchvalues.h"
#include "ui_dialoghexsignature.h"
#include "xbinary.h"
#include "xoptions.h"

DialogHexSignature::DialogHexSignature(QWidget *pParent, QIODevice *pDevice, qint64 nOffset, qint64 nSize)
    : XShortcutsDialog(pParent, false),
      ui(new Ui::DialogHexSignature),
      m_pDevice(pDevice),
      m_nOffset(nOffset)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    ui->plainTextEditSignature->setWordWrapMode(QTextOption::WrapAnywhere);
    ui->lineEditWildcard->setValidator(
        new QRegularExpressionValidator(QRegularExpression(QStringLiteral("[.?]")), ui->lineEditWildcard));
    ui->tableWidgetBytes->installEventFilter(this);
    ui->tableWidgetBytes->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidgetBytes->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidgetBytes->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);

    XOptions::setMonoFont(ui->tableWidgetBytes);
    XOptions::setMonoFont(ui->plainTextEditSignature);
    XOptions::setMonoFont(ui->lineEditWildcard);

    QStringList listColumnLabels;
    for (qint32 i = 0; i < 16; i++) {
        listColumnLabels.append(QString::number(i, 16).toUpper());
    }
    ui->tableWidgetBytes->setColumnCount(16);
    ui->tableWidgetBytes->setHorizontalHeaderLabels(listColumnLabels);

    const qint64 nSafeRequestedSize = qMax<qint64>(0, nSize);
    const qint64 nReadSize = qMin<qint64>(nSafeRequestedSize, G_N_MAX_BYTES);
    if (m_pDevice && (m_nOffset >= 0) && (nReadSize > 0)) {
        m_baData = XBinary::read_array(m_pDevice, m_nOffset, nReadSize);
    }

    const qint32 nDataSize = m_baData.size();
    const qint32 nRows = (nDataSize + 15) / 16;
    ui->tableWidgetBytes->setRowCount(nRows);

    for (qint32 nRow = 0; nRow < nRows; nRow++) {
        QTableWidgetItem *pHeaderItem = new QTableWidgetItem(QString("+%1").arg(nRow * 16, 2, 16, QChar('0')).toUpper());
        pHeaderItem->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetBytes->setVerticalHeaderItem(nRow, pHeaderItem);

        for (qint32 nColumn = 0; nColumn < 16; nColumn++) {
            const qint32 nIndex = nRow * 16 + nColumn;
            if (nIndex >= nDataSize) {
                break;
            }

            QTableWidgetItem *pItem = new QTableWidgetItem;
            pItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            pItem->setTextAlignment(Qt::AlignCenter);
            pItem->setData(ITEM_ROLE_BYTE_VALUE, static_cast<quint8>(m_baData.at(nIndex)));
            pItem->setData(ITEM_ROLE_WILDCARD, false);
            ui->tableWidgetBytes->setItem(nRow, nColumn, pItem);
            updateByteItem(pItem);
        }
    }

    const QString sOffset = (m_nOffset >= 0) ? QString::number(m_nOffset, 16).toUpper() : tr("invalid");
    if (nSafeRequestedSize <= 0) {
        ui->labelSelectionSummary->setText(tr("No bytes were selected."));
    } else if (m_nOffset < 0) {
        ui->labelSelectionSummary->setText(tr("The selected source offset is invalid."));
    } else if (!m_pDevice) {
        ui->labelSelectionSummary->setText(tr("The source device is unavailable."));
    } else if (m_baData.isEmpty()) {
        ui->labelSelectionSummary->setText(tr("No bytes could be read at offset 0x%1.").arg(sOffset));
    } else if (nSafeRequestedSize > G_N_MAX_BYTES) {
        ui->labelSelectionSummary->setText(
            tr("Offset 0x%1 — showing the first %2 bytes of %3 selected (limit: %4).")
                .arg(sOffset)
                .arg(nDataSize)
                .arg(nSafeRequestedSize)
                .arg(G_N_MAX_BYTES));
    } else if (nDataSize < nSafeRequestedSize) {
        ui->labelSelectionSummary->setText(
            tr("Offset 0x%1 — read %2 bytes of %3 selected.").arg(sOffset).arg(nDataSize).arg(nSafeRequestedSize));
    } else {
        ui->labelSelectionSummary->setText(tr("Offset 0x%1 — selected bytes: %2.").arg(sOffset).arg(nDataSize));
    }

    adjustView();
    reload();

    if (!m_baData.isEmpty()) {
        ui->tableWidgetBytes->setCurrentCell(0, 0);
        ui->tableWidgetBytes->setFocus(Qt::OtherFocusReason);
    }
}

DialogHexSignature::~DialogHexSignature()
{
    delete ui;
}

void DialogHexSignature::adjustView()
{
    const QFontMetrics metrics(ui->tableWidgetBytes->font());
    const qint32 nColumnWidth = qMax(34, metrics.horizontalAdvance(QStringLiteral("??")) + 20);
    const qint32 nRowHeight = qMax(26, metrics.height() + 10);

    for (qint32 i = 0; i < ui->tableWidgetBytes->columnCount(); i++) {
        ui->tableWidgetBytes->setColumnWidth(i, nColumnWidth);
    }
    ui->tableWidgetBytes->verticalHeader()->setDefaultSectionSize(nRowHeight);
    ui->tableWidgetBytes->horizontalHeader()->setMinimumSectionSize(nColumnWidth);

    const qint32 nVisibleRows = qBound(2, ui->tableWidgetBytes->rowCount(), 5);
    const qint32 nTableHeight = ui->tableWidgetBytes->horizontalHeader()->sizeHint().height() + nVisibleRows * nRowHeight +
                               2 * ui->tableWidgetBytes->frameWidth() + 4;
    ui->tableWidgetBytes->setMinimumHeight(nTableHeight);
    ui->tableWidgetBytes->setMaximumHeight(nTableHeight);

}

QString DialogHexSignature::buildSignature() const
{
    QStringList listTokens;
    QString sAnsiRun;

    const bool bSpaces = ui->checkBoxSpaces->isChecked();
    const bool bUpper = ui->checkBoxUpper->isChecked();
    const bool bANSI = ui->checkBoxANSI->isChecked();
    const QString sWildcard = ui->lineEditWildcard->text();

    const auto flushAnsiRun = [&]() {
        if (!sAnsiRun.isEmpty()) {
            listTokens.append(QStringLiteral("'") + sAnsiRun + QStringLiteral("'"));
            sAnsiRun.clear();
        }
    };

    for (qint32 i = 0; i < m_baData.size(); i++) {
        QTableWidgetItem *pItem = ui->tableWidgetBytes->item(i / 16, i % 16);
        const bool bWildcard = pItem && pItem->data(ITEM_ROLE_WILDCARD).toBool();
        const quint8 nByte = static_cast<quint8>(m_baData.at(i));
        const bool bPrintableAnsi = bANSI && !bWildcard && (nByte >= 0x20) && (nByte < 0x7F) && (nByte != 0x27) && (nByte != 0x22) &&
                                     (nByte != 0x5C);

        if (bPrintableAnsi) {
            sAnsiRun.append(QChar::fromLatin1(static_cast<char>(nByte)));
            continue;
        }

        flushAnsiRun();
        if (bWildcard) {
            listTokens.append(sWildcard + sWildcard);
        } else {
            QString sHex = QString("%1").arg(nByte, 2, 16, QChar('0'));
            if (bUpper) {
                sHex = sHex.toUpper();
            }
            listTokens.append(sHex);
        }
    }

    flushAnsiRun();
    return bSpaces ? listTokens.join(QChar(' ')) : listTokens.join(QString());
}

void DialogHexSignature::setWildcardState(QTableWidgetItem *pItem, bool bState)
{
    if (!pItem || !(pItem->flags() & Qt::ItemIsEnabled)) {
        return;
    }

    pItem->setData(ITEM_ROLE_WILDCARD, bState);
    updateByteItem(pItem);
}

void DialogHexSignature::updateByteItem(QTableWidgetItem *pItem)
{
    if (!pItem) {
        return;
    }

    const qint32 nRow = pItem->row();
    const qint32 nColumn = pItem->column();
    const qint32 nIndex = nRow * 16 + nColumn;
    const quint8 nByte = static_cast<quint8>(pItem->data(ITEM_ROLE_BYTE_VALUE).toUInt());
    const bool bWildcard = pItem->data(ITEM_ROLE_WILDCARD).toBool();
    const QString sHex = QString("%1").arg(nByte, 2, 16, QChar('0')).toUpper();
    const QString sAbsoluteOffset = (m_nOffset >= 0) ? QString::number(m_nOffset + nIndex, 16).toUpper() : tr("invalid");
    QFont font = pItem->font();
    font.setBold(bWildcard);
    pItem->setFont(font);

    if (bWildcard) {
        QString sToken = ui->lineEditWildcard->text().repeated(2);
        if (sToken.isEmpty()) {
            sToken = QStringLiteral("--");
        }
        pItem->setText(sToken);
        pItem->setToolTip(tr("Byte %1 at offset 0x%2, original value 0x%3: wildcarded.").arg(nIndex).arg(sAbsoluteOffset, sHex));
        pItem->setData(Qt::AccessibleTextRole, tr("Byte %1, original value %2, wildcarded").arg(nIndex).arg(sHex));
    } else {
        pItem->setText(sHex);
        pItem->setToolTip(tr("Byte %1 at offset 0x%2, value 0x%3: exact match.").arg(nIndex).arg(sAbsoluteOffset, sHex));
        pItem->setData(Qt::AccessibleTextRole, tr("Byte %1, value %2, exact match").arg(nIndex).arg(sHex));
    }
}

qint32 DialogHexSignature::getWildcardCount() const
{
    qint32 nResult = 0;

    for (qint32 i = 0; i < m_baData.size(); i++) {
        const QTableWidgetItem *pItem = ui->tableWidgetBytes->item(i / 16, i % 16);
        if (pItem && pItem->data(ITEM_ROLE_WILDCARD).toBool()) {
            nResult++;
        }
    }

    return nResult;
}

bool DialogHexSignature::isSignatureValid(const QString &sSignature) const
{
    if (m_baData.isEmpty() || sSignature.isEmpty()) {
        return false;
    }
    if (getWildcardCount() > 0) {
        const QString sWildcard = ui->lineEditWildcard->text();
        if ((sWildcard != QStringLiteral("?")) && (sWildcard != QStringLiteral("."))) {
            return false;
        }
    }

    return XBinary::isSignatureValid(sSignature);
}

void DialogHexSignature::reload()
{
    for (qint32 i = 0; i < m_baData.size(); i++) {
        updateByteItem(ui->tableWidgetBytes->item(i / 16, i % 16));
    }

    const QString sSignature = buildSignature();
    const qint32 nWildcardCount = getWildcardCount();
    const bool bValid = isSignatureValid(sSignature);

    ui->plainTextEditSignature->setPlainText(sSignature);
    ui->pushButtonClearWildcards->setEnabled(nWildcardCount > 0);
    ui->pushButtonCopy->setEnabled(bValid);
    ui->pushButtonScan->setEnabled(bValid && m_pDevice);

    if (m_baData.isEmpty()) {
        ui->labelStatus->setText(tr("No signature can be generated because no bytes were read."));
    } else if ((nWildcardCount > 0) && ui->lineEditWildcard->text().isEmpty()) {
        ui->labelStatus->setText(tr("Enter ? or . as the wildcard character for the selected bytes."));
    } else if (!bValid) {
        ui->labelStatus->setText(tr("The generated signature is invalid. Choose a supported wildcard character."));
    } else {
        ui->labelStatus->setText(
            tr("Bytes: %1; wildcarded: %2. The signature is valid.").arg(m_baData.size()).arg(nWildcardCount));
    }
}

void DialogHexSignature::on_pushButtonCopy_clicked()
{
    const QString sSignature = ui->plainTextEditSignature->toPlainText();
    if (!isSignatureValid(sSignature)) {
        return;
    }

    QApplication::clipboard()->setText(sSignature);
    ui->labelStatus->setText(tr("Signature copied to the clipboard."));
}

void DialogHexSignature::on_checkBoxSpaces_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)
    reload();
}

void DialogHexSignature::on_checkBoxUpper_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)
    reload();
}

void DialogHexSignature::on_lineEditWildcard_textChanged(const QString &sText)
{
    Q_UNUSED(sText)
    reload();
}

void DialogHexSignature::on_pushButtonScan_clicked()
{
    const QString sSignature = ui->plainTextEditSignature->toPlainText();
    if (!m_pDevice || !isSignatureValid(sSignature)) {
        return;
    }

    SearchValuesWidget::OPTIONS options = {};
    options.fileType = XBinary::FT_UNKNOWN;
    options.valueType = XBinary::VT_SIGNATURE;
    options.endian = XBinary::ENDIAN_UNKNOWN;
    options.varValue = sSignature;
    options.bScan = true;

    DialogSearchValues dialogSearchValues(this);
    dialogSearchValues.setGlobal(getShortcuts(), getGlobalOptions());
    dialogSearchValues.setData(m_pDevice, options);
    dialogSearchValues.exec();
}

void DialogHexSignature::on_checkBoxANSI_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)
    reload();
}

void DialogHexSignature::on_pushButtonClearWildcards_clicked()
{
    for (qint32 i = 0; i < m_baData.size(); i++) {
        setWildcardState(ui->tableWidgetBytes->item(i / 16, i % 16), false);
    }
    reload();
}

void DialogHexSignature::on_tableWidgetBytes_cellClicked(qint32 nRow, qint32 nColumn)
{
    QTableWidgetItem *pItem = ui->tableWidgetBytes->item(nRow, nColumn);
    if (!pItem) {
        return;
    }

    setWildcardState(pItem, !pItem->data(ITEM_ROLE_WILDCARD).toBool());
    reload();
}

bool DialogHexSignature::eventFilter(QObject *pWatched, QEvent *pEvent)
{
    if (pWatched == ui->tableWidgetBytes) {
        if (pEvent->type() == QEvent::KeyPress) {
            QKeyEvent *pKeyEvent = static_cast<QKeyEvent *>(pEvent);
            if (pKeyEvent->key() == Qt::Key_Space) {
                QTableWidgetItem *pItem = ui->tableWidgetBytes->currentItem();
                if (pItem) {
                    setWildcardState(pItem, !pItem->data(ITEM_ROLE_WILDCARD).toBool());
                    reload();
                }
                return true;
            }
        }
        return false;
    }

    return XShortcutsDialog::eventFilter(pWatched, pEvent);
}

void DialogHexSignature::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
}
