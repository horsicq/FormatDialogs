/* Copyright (c) 2023-2026 hors<horsicq@gmail.com>
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
#include "dialogshowdata.h"

#include <QListWidgetItem>
#include <QScrollBar>

#include "ui_dialogshowdata.h"

namespace {
const qint64 MAX_DATA_SIZE = 0x10000;

QString joinTokens(const QStringList &listTokens, qint32 nBegin, qint32 nCount)
{
    QStringList listLine;
    listLine.reserve(nCount);

    for (qint32 i = 0; i < nCount; i++) {
        listLine.append(listTokens.at(nBegin + i));
    }

    return listLine.join(QStringLiteral(", "));
}
}  // namespace

DialogShowData::DialogShowData(QWidget *pParent, QIODevice *pDevice, qint64 nOffset, qint64 nSize)
    : XShortcutsDialog(pParent, true), ui(new Ui::DialogShowData), m_nOffset(nOffset)
{
    ui->setupUi(this);

    _addItem(tr("Hex"), DTYPE_HEX);
    _addItem(tr("Plain Text"), DTYPE_PLAINTEXT);
    _addItem("C", DTYPE_C);
    _addItem("C++", DTYPE_CPP);
    _addItem("MASM", DTYPE_MASM);
    _addItem("FASM", DTYPE_FASM);
    _addItem("Java", DTYPE_JAVA);
    _addItem("JavaScript", DTYPE_JAVASCRIPT);
    _addItem("Python", DTYPE_PYTHON);
    _addItem("C#", DTYPE_CSHARP);
    _addItem("VB.NET", DTYPE_VBNET);
    _addItem("Rust", DTYPE_RUST);
    _addItem("Pascal", DTYPE_PASCAL);
    _addItem("Lua", DTYPE_LUA);
    _addItem("Go", DTYPE_GO);
    _addItem("Crystal", DTYPE_CRYSTAL);
    _addItem("Swift", DTYPE_SWIFT);
    _addItem("Base64", DTYPE_BASE64);

    ui->spinBoxElementsProLine->blockSignals(true);
    ui->checkBoxGroup->blockSignals(true);
    ui->spinBoxElementsProLine->setValue(16);
    ui->checkBoxGroup->setChecked(true);
    ui->spinBoxElementsProLine->blockSignals(false);
    ui->checkBoxGroup->blockSignals(false);

    XOptions::setMonoFont(ui->plainTextEditData);
    ui->plainTextEditData->setPlaceholderText(tr("No readable data is available for this selection."));
    ui->splitter->setStretchFactor(0, 0);
    ui->splitter->setStretchFactor(1, 1);
    ui->splitter->setChildrenCollapsible(false);

    loadData(pDevice, nSize);
    ui->labelSelectionSummary->setText(m_sSelectionSummary);

    ui->listWidgetType->setCurrentRow(0);
    adjustView();
    if (m_baData.isEmpty()) {
        ui->listWidgetType->setEnabled(false);
        ui->plainTextEditData->setEnabled(false);
        ui->pushButtonOK->setFocus(Qt::OtherFocusReason);
    } else {
        ui->listWidgetType->setFocus(Qt::OtherFocusReason);
    }
}

DialogShowData::~DialogShowData()
{
    delete ui;
}

void DialogShowData::adjustView()
{
    const qint32 nFormatWidth =
        ui->listWidgetType->sizeHintForColumn(0) + ui->listWidgetType->frameWidth() * 2 + ui->listWidgetType->verticalScrollBar()->sizeHint().width() + 28;
    ui->widgetFormat->setMinimumWidth(qMax(150, nFormatWidth));
    ui->splitter->setSizes(QList<int>() << qMax(150, nFormatWidth) << qMax(480, width() - nFormatWidth));

    setMinimumSize(qMax(560, minimumSizeHint().width()), qMax(320, minimumSizeHint().height()));
}

void DialogShowData::on_pushButtonOK_clicked()
{
    close();
}

void DialogShowData::loadData(QIODevice *pDevice, qint64 nSize)
{
    m_baData.clear();

    if (!pDevice) {
        m_sSelectionSummary = tr("The source device is unavailable.");
        return;
    }
    if ((m_nOffset < 0) || (nSize < -1)) {
        m_sSelectionSummary = tr("The selected source range is invalid.");
        return;
    }
    if (!pDevice->isReadable()) {
        m_sSelectionSummary = tr("The source device is not readable.");
        return;
    }

    const qint64 nDeviceSize = pDevice->size();
    if ((nDeviceSize < 0) || (m_nOffset > nDeviceSize)) {
        m_sSelectionSummary = tr("The selected source range is outside the available data.");
        return;
    }

    const qint64 nAvailableSize = nDeviceSize - m_nOffset;
    const qint64 nSelectionSize = (nSize == -1) ? nAvailableSize : nSize;
    if (nSelectionSize == 0) {
        m_sSelectionSummary = tr("No bytes were selected.");
        return;
    }

    const qint64 nTargetSize = qMin(nSelectionSize, MAX_DATA_SIZE);
    const qint64 nReadSize = qMin(nTargetSize, nAvailableSize);
    if (nReadSize <= 0) {
        m_sSelectionSummary = tr("No bytes are available at offset 0x%1.").arg(QString::number(m_nOffset, 16).toUpper());
        return;
    }

    const qint64 nOriginalPosition = pDevice->pos();
    m_baData = XBinary::read_array(pDevice, m_nOffset, nReadSize);
    const bool bPositionRestored = (nOriginalPosition < 0) || pDevice->seek(nOriginalPosition);
    const QString sOffset = QString::number(m_nOffset, 16).toUpper();

    if (m_baData.isEmpty()) {
        m_sSelectionSummary = tr("No bytes could be read at offset 0x%1.").arg(sOffset);
    } else if (m_baData.size() < nTargetSize) {
        m_sSelectionSummary = tr("Offset 0x%1 — read %2 of %3 requested bytes.").arg(sOffset).arg(m_baData.size()).arg(nSelectionSize);
    } else if (nSelectionSize > MAX_DATA_SIZE) {
        m_sSelectionSummary =
            tr("Offset 0x%1 — showing the first %2 of %3 selected bytes (limit: %4).").arg(sOffset).arg(m_baData.size()).arg(nSelectionSize).arg(MAX_DATA_SIZE);
    } else {
        m_sSelectionSummary = tr("Offset 0x%1 — showing %2 bytes.").arg(sOffset).arg(m_baData.size());
    }

    if (!bPositionRestored) {
        m_sSelectionSummary += QLatin1Char(' ');
        m_sSelectionSummary += tr("The source position could not be restored.");
    }
}

void DialogShowData::reload()
{
    QListWidgetItem *pCurrentItem = ui->listWidgetType->currentItem();
    if (!pCurrentItem) {
        ui->plainTextEditData->clear();
        ui->pushButtonCopy->setEnabled(false);
        return;
    }

    const DTYPE dtype = static_cast<DTYPE>(pCurrentItem->data(Qt::UserRole).toUInt());
    const bool bHasData = !m_baData.isEmpty();
    const bool bSupportsWrapping = bHasData && (dtype != DTYPE_BASE64) && (dtype != DTYPE_PLAINTEXT);
    const bool bWrap = bSupportsWrapping && ui->checkBoxGroup->isChecked();

    ui->checkBoxGroup->setEnabled(bSupportsWrapping);
    ui->labelElementsProLine->setEnabled(bWrap);
    ui->spinBoxElementsProLine->setEnabled(bWrap);

    const QString sData = getDataString(dtype);
    ui->plainTextEditData->setPlainText(sData);
    ui->pushButtonCopy->setEnabled(bHasData && !sData.isEmpty());
}

void DialogShowData::on_pushButtonCopy_clicked()
{
    QApplication::clipboard()->setText(ui->plainTextEditData->toPlainText());
}

void DialogShowData::on_listWidgetType_currentRowChanged(int nCurrentRow)
{
    Q_UNUSED(nCurrentRow)

    reload();
}

QString DialogShowData::getDataString(DTYPE dtype) const
{
    if (m_baData.isEmpty()) {
        return QString();
    }
    if (dtype == DTYPE_BASE64) {
        return QString::fromLatin1(m_baData.toBase64());
    }
    if (dtype == DTYPE_PLAINTEXT) {
        return XBinary::dataToString(m_baData, XBinary::DSMODE_NONE);
    }

    const qint32 nDataSize = m_baData.size();
    const qint32 nElementsProLine = qMax(1, ui->spinBoxElementsProLine->value());
    const bool bIsGroup = ui->checkBoxGroup->isChecked();
    QStringList listTokens;
    listTokens.reserve(nDataSize);

    for (qint32 i = 0; i < nDataSize; i++) {
        const quint8 nByte = static_cast<quint8>(m_baData.at(i));
        const QString sHex = XBinary::valueToHex(nByte).toUpper();
        QString sToken;

        if (dtype == DTYPE_HEX) {
            sToken = sHex;
        } else if (dtype == DTYPE_VBNET) {
            sToken = QStringLiteral("&H") + sHex;
        } else if (dtype == DTYPE_PASCAL) {
            sToken = QStringLiteral("$") + sHex;
        } else if (dtype == DTYPE_MASM) {
            sToken = ((nByte >= 0xA0) ? QStringLiteral("0") : QString()) + sHex + QLatin1Char('h');
        } else {
            sToken = QStringLiteral("0x") + sHex;
            if ((dtype == DTYPE_JAVA) && (nByte >= 0x80)) {
                sToken.prepend(QStringLiteral("(byte) "));
            }
        }

        listTokens.append(sToken);
    }

    if (dtype == DTYPE_HEX) {
        if (!bIsGroup) {
            return listTokens.join(QLatin1Char(' '));
        }

        QStringList listLines;
        for (qint32 nBegin = 0; nBegin < nDataSize; nBegin += nElementsProLine) {
            const qint32 nCount = qMin(nElementsProLine, nDataSize - nBegin);
            listLines.append(listTokens.mid(nBegin, nCount).join(QLatin1Char(' ')));
        }
        return listLines.join(QLatin1Char('\n'));
    }

    if ((dtype == DTYPE_MASM) || (dtype == DTYPE_FASM)) {
        const QString sDirective = (dtype == DTYPE_MASM) ? QStringLiteral("DB") : QStringLiteral("db");
        if (!bIsGroup) {
            return QStringLiteral("data: %1 %2").arg(sDirective, listTokens.join(QStringLiteral(", ")));
        }

        QStringList listLines;
        for (qint32 nBegin = 0; nBegin < nDataSize; nBegin += nElementsProLine) {
            const qint32 nCount = qMin(nElementsProLine, nDataSize - nBegin);
            const QString sLabel = (nBegin == 0) ? QStringLiteral("data: ") : QStringLiteral("      ");
            listLines.append(sLabel + sDirective + QLatin1Char(' ') + joinTokens(listTokens, nBegin, nCount));
        }
        return listLines.join(QLatin1Char('\n'));
    }

    QString sPrefix;
    QString sSuffix;
    if (dtype == DTYPE_C) {
        sPrefix = QStringLiteral("const uint8_t data[%1] = {").arg(nDataSize);
        sSuffix = QStringLiteral("};");
    } else if (dtype == DTYPE_CPP) {
        sPrefix = QStringLiteral("constexpr std::array<uint8_t, %1> data = {").arg(nDataSize);
        sSuffix = QStringLiteral("};");
    } else if (dtype == DTYPE_JAVA) {
        sPrefix = QStringLiteral("final byte[] data = {");
        sSuffix = QStringLiteral("};");
    } else if (dtype == DTYPE_JAVASCRIPT) {
        sPrefix = QStringLiteral("const data = new Uint8Array([");
        sSuffix = QStringLiteral("]);");
    } else if (dtype == DTYPE_PYTHON) {
        sPrefix = QStringLiteral("data = bytes([");
        sSuffix = QStringLiteral("])");
    } else if (dtype == DTYPE_CSHARP) {
        sPrefix = QStringLiteral("byte[] data = {");
        sSuffix = QStringLiteral("};");
    } else if (dtype == DTYPE_VBNET) {
        sPrefix = QStringLiteral("Dim data As Byte() = {");
        sSuffix = QStringLiteral("}");
    } else if (dtype == DTYPE_RUST) {
        sPrefix = QStringLiteral("let data: [u8; %1] = [").arg(nDataSize);
        sSuffix = QStringLiteral("];");
    } else if (dtype == DTYPE_PASCAL) {
        sPrefix = QStringLiteral("const\n  data: array[0..%1] of Byte = (").arg(nDataSize - 1);
        sSuffix = QStringLiteral(");");
    } else if (dtype == DTYPE_LUA) {
        sPrefix = QStringLiteral("data = {");
        sSuffix = QStringLiteral("}");
    } else if (dtype == DTYPE_GO) {
        sPrefix = QStringLiteral("var data = [...]byte{");
        sSuffix = QStringLiteral("}");
    } else if (dtype == DTYPE_CRYSTAL) {
        sPrefix = QStringLiteral("data = [");
        sSuffix = QStringLiteral("] of UInt8");
    } else if (dtype == DTYPE_SWIFT) {
        sPrefix = QStringLiteral("let data: [UInt8] = [");
        sSuffix = QStringLiteral("]");
    } else {
        return QString();
    }

    if (!bIsGroup) {
        return sPrefix + QLatin1Char(' ') + listTokens.join(QStringLiteral(", ")) + QLatin1Char(' ') + sSuffix;
    }

    QString sResult = sPrefix + QLatin1Char('\n');
    for (qint32 nBegin = 0; nBegin < nDataSize; nBegin += nElementsProLine) {
        const qint32 nCount = qMin(nElementsProLine, nDataSize - nBegin);
        const bool bLastLine = (nBegin + nCount) == nDataSize;
        sResult += QStringLiteral("    ") + joinTokens(listTokens, nBegin, nCount);
        if (!bLastLine || (dtype == DTYPE_GO)) {
            sResult += QLatin1Char(',');
        }
        sResult += QLatin1Char('\n');
    }
    sResult += sSuffix;

    return sResult;
}

void DialogShowData::_addItem(const QString &sName, DTYPE dtype)
{
    QListWidgetItem *pItem = new QListWidgetItem;
    pItem->setText(sName);
    pItem->setData(Qt::UserRole, dtype);

    ui->listWidgetType->addItem(pItem);
}

void DialogShowData::on_spinBoxElementsProLine_valueChanged(int nArg)
{
    Q_UNUSED(nArg)

    reload();
}

void DialogShowData::on_checkBoxGroup_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)

    reload();
}

void DialogShowData::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
}
