/* Copyright (c) 2023-2026 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "dialogdatainspector.h"

#include <limits>

#include <QHeaderView>
#include <QScopedValueRollback>
#include <QSignalBlocker>
#include <QTimer>

#include "ui_dialogdatainspector.h"

namespace {
constexpr qint64 MAX_TEXT_BYTES = 0x10000;

quint64 readUnsigned(const QByteArray &baData, qint32 nOffset, qint32 nWidth, bool bIsBigEndian)
{
    quint64 nResult = 0;

    for (qint32 i = 0; i < nWidth; ++i) {
        const qint32 nIndex = bIsBigEndian ? i : (nWidth - i - 1);
        nResult = (nResult << 8) | static_cast<quint8>(baData.at(nOffset + nIndex));
    }

    return nResult;
}

QByteArray writeUnsigned(quint64 nValue, qint32 nWidth, bool bIsBigEndian)
{
    QByteArray baResult(nWidth, 0);

    for (qint32 i = 0; i < nWidth; ++i) {
        const qint32 nIndex = bIsBigEndian ? (nWidth - i - 1) : i;
        baResult[nIndex] = static_cast<char>((nValue >> (i * 8)) & 0xFF);
    }

    return baResult;
}

QString readAnsiString(const QByteArray &baData)
{
    qint32 nLength = 0;

    while ((nLength < baData.size()) && (baData.at(nLength) != 0)) {
        ++nLength;
    }

    return QString::fromLatin1(baData.constData(), nLength);
}

QString readUnicodeString(const QByteArray &baData, bool bIsBigEndian)
{
    QString sResult;
    const qint32 nWords = baData.size() / 2;
    sResult.reserve(nWords);

    for (qint32 i = 0; i < nWords; ++i) {
        const quint16 nValue = static_cast<quint16>(readUnsigned(baData, i * 2, 2, bIsBigEndian));

        if (nValue == 0) {
            break;
        }

        sResult.append(QChar(nValue));
    }

    return sResult;
}

qint64 readExactAt(const QPointer<QIODevice> &pDevice, qint64 nOffset, char *pData, qint64 nSize)
{
    if (!pDevice || !pData || (nOffset < 0) || (nSize < 0) || (nSize > (std::numeric_limits<qint64>::max)() - nOffset)) {
        return 0;
    }

    qint64 nRead = 0;

    while (pDevice && (nRead < nSize)) {
        if (!pDevice->seek(nOffset + nRead)) {
            break;
        }
        if (!pDevice) {
            break;
        }

        const qint64 nCurrent = pDevice->read(pData + nRead, nSize - nRead);
        if ((nCurrent <= 0) || (nCurrent > nSize - nRead)) {
            break;
        }

        nRead += nCurrent;
    }

    return nRead;
}

qint64 writeExactAt(const QPointer<QIODevice> &pDevice, qint64 nOffset, const char *pData, qint64 nSize)
{
    if (!pDevice || !pData || (nOffset < 0) || (nSize < 0) || (nSize > (std::numeric_limits<qint64>::max)() - nOffset)) {
        return 0;
    }

    qint64 nWritten = 0;

    while (pDevice && (nWritten < nSize)) {
        if (!pDevice->seek(nOffset + nWritten)) {
            break;
        }
        if (!pDevice) {
            break;
        }

        const qint64 nCurrent = pDevice->write(pData + nWritten, nSize - nWritten);
        if ((nCurrent <= 0) || (nCurrent > nSize - nWritten)) {
            break;
        }

        nWritten += nCurrent;
    }

    return nWritten;
}
}  // namespace

DialogDataInspector::DialogDataInspector(QWidget *pParent, QIODevice *pDevice, qint64 nOffset, qint64 nSize)
    : XShortcutsDialog(pParent, false), ui(new Ui::DialogDataInspector), m_pDevice(pDevice), m_nOffset(nOffset), m_nSize(nSize), m_bSync(false)
{
    ui->setupUi(this);

    memset(m_lineEdit, 0, sizeof m_lineEdit);

    ui->tableWidgetDataInspector->setColumnCount(2);
    ui->tableWidgetDataInspector->setRowCount(__DATAINS_SIZE);
    ui->tableWidgetDataInspector->setHorizontalHeaderLabels({tr("Name"), tr("Value")});
    ui->tableWidgetDataInspector->horizontalHeader()->setVisible(true);
    ui->tableWidgetDataInspector->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidgetDataInspector->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableWidgetDataInspector->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    addRecord("BYTE", DATAINS_BYTE);
    addRecord("WORD", DATAINS_WORD);
    addRecord("DWORD", DATAINS_DWORD);
    addRecord("QWORD", DATAINS_QWORD);
    addRecord("uint8", DATAINS_UINT8);
    addRecord("int8", DATAINS_INT8);
    addRecord("uint16", DATAINS_UINT16);
    addRecord("int16", DATAINS_INT16);
    addRecord("uint32", DATAINS_UINT32);
    addRecord("int32", DATAINS_INT32);
    addRecord("uint64", DATAINS_UINT64);
    addRecord("int64", DATAINS_INT64);
    addRecord("ANSI", DATAINS_ANSI);
    addRecord(tr("Unicode"), DATAINS_UNICODE);
    addRecord(tr("Binary"), DATAINS_BINARY);

    {
        const QSignalBlocker blocker(ui->comboBoxEndianness);
        XFormats::setEndiannessComboBox(ui->comboBoxEndianness, XBinary::ENDIAN_LITTLE);
    }
    {
        const QSignalBlocker blocker(ui->checkBoxReadonly);
        ui->checkBoxReadonly->setChecked(true);
    }

    if (pDevice) {
        connect(pDevice, &QObject::destroyed, this, [this]() {
            m_pDevice = nullptr;
            showData(m_nOffset, m_nSize);
        });
        connect(pDevice, &QIODevice::aboutToClose, this, [this]() { QTimer::singleShot(0, this, [this]() { showData(m_nOffset, m_nSize); }); });
    }

    setReadonly(true);
    showData(nOffset, nSize);
}

DialogDataInspector::~DialogDataInspector()
{
    delete ui;
}

void DialogDataInspector::adjustView()
{
    ui->tableWidgetDataInspector->resizeRowsToContents();
}

void DialogDataInspector::setReadonly(bool bState)
{
    const bool bEffectiveState = bState || !isDeviceReady(true) || (m_nSize <= 0);

    XShortcutsObject::setReadonly(bEffectiveState);

    {
        const QSignalBlocker blocker(ui->checkBoxReadonly);
        ui->checkBoxReadonly->setChecked(bEffectiveState);
        ui->checkBoxReadonly->setEnabled(isDeviceReady(true) && (m_nSize > 0));
    }

    for (qint32 i = 0; i < __DATAINS_SIZE; ++i) {
        if (m_lineEdit[i]) {
            m_lineEdit[i]->setReadOnly(bEffectiveState);
        }
    }

    if (isDeviceReady(false) && (m_nSize > 0)) {
        if (m_pDevice->openMode() & QIODevice::Append) {
            ui->labelStatus->setText(tr("Append-mode devices can be inspected but not edited."));
        } else {
            ui->labelStatus->setText(bEffectiveState ? tr("Values are shown in read-only mode.") : tr("Values can be edited."));
        }
    }
}

void DialogDataInspector::currentLocationChangedSlot(quint64 nLocation, qint32 nLocationType, qint64 nSize)
{
    if (nLocationType == XBinary::LT_OFFSET) {
        if (nLocation <= static_cast<quint64>((std::numeric_limits<qint64>::max)())) {
            showData(static_cast<qint64>(nLocation), nSize);
        } else {
            showData(-1, nSize);
        }
    }
}

void DialogDataInspector::dataChangedSlot(qint64 nOffset, qint64 nSize)
{
    if ((nOffset < 0) || (nSize <= 0) || (m_nOffset < 0) || (m_nSize <= 0)) {
        return;
    }

    bool bOverlaps = false;

    if (nOffset <= m_nOffset) {
        bOverlaps = nSize > (m_nOffset - nOffset);
    } else {
        bOverlaps = m_nSize > (nOffset - m_nOffset);
    }

    if (bOverlaps) {
        showData(m_nOffset, m_nSize);
    }
}

void DialogDataInspector::addRecord(const QString &sTitle, const DATAINS &datains)
{
    QTableWidgetItem *pItemName = new QTableWidgetItem(sTitle);
    pItemName->setFlags(pItemName->flags() & ~Qt::ItemIsEditable);
    pItemName->setToolTip(tr("%1 interpretation").arg(sTitle));
    ui->tableWidgetDataInspector->setItem(datains, 0, pItemName);

    XLineEditHEX *pLineEdit = new XLineEditHEX(ui->tableWidgetDataInspector);
    pLineEdit->setObjectName(QStringLiteral("lineEditValue_%1").arg(static_cast<qint32>(datains)));
    pLineEdit->setProperty("STYPE", datains);
    pLineEdit->setAccessibleName(tr("%1 value").arg(sTitle));
    pLineEdit->setAccessibleDescription(tr("The selected bytes interpreted as %1.").arg(sTitle));
    pLineEdit->setToolTip(tr("The selected bytes interpreted as %1.").arg(sTitle));
    m_lineEdit[datains] = pLineEdit;

    connect(pLineEdit, SIGNAL(valueChanged(QVariant)), this, SLOT(valueChangedSlot(QVariant)));
    ui->tableWidgetDataInspector->setCellWidget(datains, 1, pLineEdit);
}

void DialogDataInspector::blockSignals(bool bState)
{
    for (qint32 i = 0; i < __DATAINS_SIZE; ++i) {
        if (m_lineEdit[i]) {
            m_lineEdit[i]->blockSignals(bState);
        }
    }
}

void DialogDataInspector::clearValues()
{
    blockSignals(true);

    for (qint32 i = 0; i < __DATAINS_SIZE; ++i) {
        enableRow(i, false);
    }

    blockSignals(false);
}

bool DialogDataInspector::isDeviceReady(bool bRequireWritable) const
{
    QIODevice *pDevice = m_pDevice.data();
    if (!pDevice || (pDevice->thread() != thread())) {
        return false;
    }

    const QIODevice::OpenMode openMode = pDevice->openMode();

    return pDevice->isOpen() && pDevice->isReadable() && !pDevice->isSequential() && !(openMode & QIODevice::Text) && (pDevice->size() >= 0) &&
           (!bRequireWritable || (pDevice->isWritable() && !(openMode & QIODevice::Append) && !(openMode & QIODevice::Text)));
}

void DialogDataInspector::showData(qint64 nOffset, qint64 nSize)
{
    m_nOffset = nOffset;
    m_nSize = 0;

    if (nOffset >= 0) {
        ui->lineEditOffset->setValue32_64(static_cast<quint64>(nOffset));
    } else {
        ui->lineEditOffset->clear();
    }

    if (nSize >= 0) {
        ui->lineEditSize->setValue32_64(static_cast<quint64>(nSize));
    } else {
        ui->lineEditSize->clear();
    }

    if (!isDeviceReady(false)) {
        clearValues();
        ui->comboBoxEndianness->setEnabled(false);
        setReadonly(true);
        QIODevice *pUnavailableDevice = m_pDevice.data();
        QString sStatus;

        if (!pUnavailableDevice) {
            sStatus = tr("No data device is available.");
        } else if (pUnavailableDevice->thread() != thread()) {
            sStatus = tr("The data device belongs to a different thread.");
        } else if (!pUnavailableDevice->isOpen() || !pUnavailableDevice->isReadable()) {
            sStatus = tr("An open, readable data device is required.");
        } else if (pUnavailableDevice->isSequential()) {
            sStatus = tr("Sequential devices cannot be inspected by byte offset.");
        } else if (pUnavailableDevice->openMode() & QIODevice::Text) {
            sStatus = tr("Text-mode devices cannot be inspected by byte offset.");
        } else {
            sStatus = tr("The data device does not report a usable size.");
        }

        ui->labelStatus->setText(sStatus);
        return;
    }

    QPointer<QIODevice> pDevice = m_pDevice;
    const qint64 nDeviceSize = pDevice->size();

    if ((nOffset < 0) || (nOffset >= nDeviceSize) || (nSize <= 0)) {
        clearValues();
        ui->comboBoxEndianness->setEnabled(false);
        setReadonly(true);
        ui->lineEditSize->setValue32_64(0);
        ui->labelStatus->setText((nSize <= 0) ? tr("The selection is empty.") : tr("The selection starts outside the available data."));
        return;
    }

    const qint64 nAvailableSize = qMin(nSize, nDeviceSize - nOffset);
    const qint64 nReadSize = qMin(nAvailableSize, MAX_TEXT_BYTES);
    const qint64 nOriginalPosition = pDevice->pos();
    QByteArray baData;

    if (nReadSize <= (std::numeric_limits<qint32>::max)()) {
        baData.resize(static_cast<qint32>(nReadSize));
        const qint64 nRead = readExactAt(pDevice, nOffset, baData.data(), nReadSize);

        if (nRead != nReadSize) {
            baData.resize(static_cast<qint32>(qMax<qint64>(0, nRead)));
        }
    }

    const bool bPositionRestored = pDevice && (nOriginalPosition >= 0) && pDevice->seek(nOriginalPosition);

    if (!pDevice || baData.isEmpty()) {
        clearValues();
        ui->comboBoxEndianness->setEnabled(false);
        setReadonly(true);
        ui->lineEditSize->setValue32_64(0);
        QString sStatus = tr("The selected data could not be read.");
        if (!bPositionRestored) {
            sStatus += QLatin1Char(' ') + tr("The device position could not be restored.");
        }
        ui->labelStatus->setText(sStatus);
        return;
    }

    m_nSize = nAvailableSize;
    ui->lineEditSize->setValue32_64(static_cast<quint64>(m_nSize));
    ui->comboBoxEndianness->setEnabled(true);
    setReadonly(ui->checkBoxReadonly->isChecked());

    const qint64 nReadableSize = baData.size();
    const bool bIsBigEndian = (static_cast<XBinary::ENDIAN>(ui->comboBoxEndianness->currentData(Qt::UserRole).toUInt()) == XBinary::ENDIAN_BIG);

    blockSignals(true);

    static const qint32 REQUIRED_BYTES[__DATAINS_SIZE] = {1, 2, 4, 8, 1, 1, 2, 2, 4, 4, 8, 8, 1, 2, 1};
    for (qint32 i = 0; i < __DATAINS_SIZE; ++i) {
        enableRow(i, nReadableSize >= REQUIRED_BYTES[i]);
    }

    if (nReadableSize >= 1) {
        const quint8 nValue = static_cast<quint8>(readUnsigned(baData, 0, 1, false));
        if (!m_lineEdit[DATAINS_BYTE]->isFocused() || !m_bSync) m_lineEdit[DATAINS_BYTE]->setValue_uint8(nValue, XLineEditHEX::_MODE_HEX);
        if (!m_lineEdit[DATAINS_UINT8]->isFocused() || !m_bSync) m_lineEdit[DATAINS_UINT8]->setValue_uint8(nValue, XLineEditHEX::_MODE_DEC);
        if (!m_lineEdit[DATAINS_INT8]->isFocused() || !m_bSync) m_lineEdit[DATAINS_INT8]->setValue_int8(static_cast<qint8>(nValue), XLineEditHEX::_MODE_SIGN_DEC);
        if (!m_lineEdit[DATAINS_ANSI]->isFocused() || !m_bSync) m_lineEdit[DATAINS_ANSI]->setValue_String(readAnsiString(baData), baData.size());
        if (!m_lineEdit[DATAINS_BINARY]->isFocused() || !m_bSync) m_lineEdit[DATAINS_BINARY]->setValue_uint8(nValue, XLineEditHEX::_MODE_BIN);
    }

    if (nReadableSize >= 2) {
        const quint16 nValue = static_cast<quint16>(readUnsigned(baData, 0, 2, bIsBigEndian));
        if (!m_lineEdit[DATAINS_WORD]->isFocused() || !m_bSync) m_lineEdit[DATAINS_WORD]->setValue_uint16(nValue, XLineEditHEX::_MODE_HEX);
        if (!m_lineEdit[DATAINS_UINT16]->isFocused() || !m_bSync) m_lineEdit[DATAINS_UINT16]->setValue_uint16(nValue, XLineEditHEX::_MODE_DEC);
        if (!m_lineEdit[DATAINS_INT16]->isFocused() || !m_bSync) m_lineEdit[DATAINS_INT16]->setValue_int16(static_cast<qint16>(nValue), XLineEditHEX::_MODE_SIGN_DEC);
        if (!m_lineEdit[DATAINS_UNICODE]->isFocused() || !m_bSync)
            m_lineEdit[DATAINS_UNICODE]->setValue_String(readUnicodeString(baData, bIsBigEndian), baData.size() / 2);
    }

    if (nReadableSize >= 4) {
        const quint32 nValue = static_cast<quint32>(readUnsigned(baData, 0, 4, bIsBigEndian));
        if (!m_lineEdit[DATAINS_DWORD]->isFocused() || !m_bSync) m_lineEdit[DATAINS_DWORD]->setValue_uint32(nValue, XLineEditHEX::_MODE_HEX);
        if (!m_lineEdit[DATAINS_UINT32]->isFocused() || !m_bSync) m_lineEdit[DATAINS_UINT32]->setValue_uint32(nValue, XLineEditHEX::_MODE_DEC);
        if (!m_lineEdit[DATAINS_INT32]->isFocused() || !m_bSync) m_lineEdit[DATAINS_INT32]->setValue_int32(static_cast<qint32>(nValue), XLineEditHEX::_MODE_SIGN_DEC);
    }

    if (nReadableSize >= 8) {
        const quint64 nValue = readUnsigned(baData, 0, 8, bIsBigEndian);
        if (!m_lineEdit[DATAINS_QWORD]->isFocused() || !m_bSync) m_lineEdit[DATAINS_QWORD]->setValue_uint64(nValue, XLineEditHEX::_MODE_HEX);
        if (!m_lineEdit[DATAINS_UINT64]->isFocused() || !m_bSync) m_lineEdit[DATAINS_UINT64]->setValue_uint64(nValue, XLineEditHEX::_MODE_DEC);
        if (!m_lineEdit[DATAINS_INT64]->isFocused() || !m_bSync) m_lineEdit[DATAINS_INT64]->setValue_int64(static_cast<qint64>(nValue), XLineEditHEX::_MODE_SIGN_DEC);
    }

    blockSignals(false);

    QString sStatus = tr("Showing %1 bytes at offset 0x%2.").arg(m_nSize).arg(static_cast<quint64>(m_nOffset), 0, 16);
    if (nAvailableSize < nSize) {
        sStatus += QLatin1Char(' ') + tr("The selection was clipped to the available data.");
    }
    if (nAvailableSize > MAX_TEXT_BYTES) {
        sStatus += QLatin1Char(' ') + tr("Text previews are limited to the first %1 bytes.").arg(MAX_TEXT_BYTES);
    }
    if (baData.size() < nReadSize) {
        sStatus += QLatin1Char(' ') + tr("Only %1 bytes could be read.").arg(baData.size());
    }
    if (!bPositionRestored) {
        sStatus += QLatin1Char(' ') + tr("The device position could not be restored.");
    }
    if (pDevice->openMode() & QIODevice::Append) {
        sStatus += QLatin1Char(' ') + tr("Append-mode devices are view-only.");
    } else {
        sStatus += QLatin1Char(' ') + (isReadonly() ? tr("Read-only mode is active.") : tr("Values can be edited."));
    }
    ui->labelStatus->setText(sStatus);
}

void DialogDataInspector::enableRow(qint32 nRow, bool bState)
{
    if ((nRow < 0) || (nRow >= __DATAINS_SIZE) || !m_lineEdit[nRow]) {
        return;
    }

    if (ui->tableWidgetDataInspector->isRowHidden(nRow) == bState) {
        ui->tableWidgetDataInspector->setRowHidden(nRow, !bState);
    }
    if (m_lineEdit[nRow]->isEnabled() != bState) {
        m_lineEdit[nRow]->setEnabled(bState);
    }

    if (!bState && !m_lineEdit[nRow]->text().isEmpty()) {
        m_lineEdit[nRow]->clear();
    }
}

QByteArray DialogDataInspector::createWriteData(DATAINS dataType, const QVariant &varValue, bool bIsBigEndian) const
{
    qint32 nWidth = 0;
    quint64 nValue = 0;

    switch (dataType) {
        case DATAINS_BYTE:
        case DATAINS_UINT8:
        case DATAINS_BINARY:
            nWidth = 1;
            nValue = varValue.toULongLong();
            break;
        case DATAINS_INT8:
            nWidth = 1;
            nValue = static_cast<quint8>(static_cast<qint8>(varValue.toLongLong()));
            break;
        case DATAINS_WORD:
        case DATAINS_UINT16:
            nWidth = 2;
            nValue = varValue.toULongLong();
            break;
        case DATAINS_INT16:
            nWidth = 2;
            nValue = static_cast<quint16>(static_cast<qint16>(varValue.toLongLong()));
            break;
        case DATAINS_DWORD:
        case DATAINS_UINT32:
            nWidth = 4;
            nValue = varValue.toULongLong();
            break;
        case DATAINS_INT32:
            nWidth = 4;
            nValue = static_cast<quint32>(static_cast<qint32>(varValue.toLongLong()));
            break;
        case DATAINS_QWORD:
        case DATAINS_UINT64:
            nWidth = 8;
            nValue = varValue.toULongLong();
            break;
        case DATAINS_INT64:
            nWidth = 8;
            nValue = static_cast<quint64>(varValue.toLongLong());
            break;
        case DATAINS_ANSI: {
            const qint32 nCapacity = static_cast<qint32>(qMin(m_nSize, MAX_TEXT_BYTES));
            if (nCapacity <= 0) {
                return QByteArray();
            }

            QByteArray baResult = varValue.toString().toLatin1().left(nCapacity - 1);
            baResult.append('\0');
            return baResult;
        }
        case DATAINS_UNICODE: {
            const QString sValue = varValue.toString();
            const qint32 nCapacity = static_cast<qint32>(qMin(m_nSize / 2, MAX_TEXT_BYTES / 2));
            qint32 nCharacters = qMin(sValue.size(), qMax(0, nCapacity - 1));
            if ((nCharacters > 0) && (nCharacters < sValue.size()) && sValue.at(nCharacters - 1).isHighSurrogate() && sValue.at(nCharacters).isLowSurrogate()) {
                --nCharacters;
            }
            QByteArray baResult;
            baResult.reserve((nCharacters + 1) * 2);

            for (qint32 i = 0; i < nCharacters; ++i) {
                baResult.append(writeUnsigned(sValue.at(i).unicode(), 2, bIsBigEndian));
            }

            if (nCapacity > 0) {
                baResult.append(writeUnsigned(0, 2, bIsBigEndian));
            }

            return baResult;
        }
        default: return QByteArray();
    }

    if ((nWidth <= 0) || (m_nSize < nWidth)) {
        return QByteArray();
    }

    return writeUnsigned(nValue, nWidth, bIsBigEndian);
}

void DialogDataInspector::valueChangedSlot(QVariant varValue)
{
    XLineEditHEX *pLineEdit = qobject_cast<XLineEditHEX *>(sender());

    if (!pLineEdit || m_bSync || !pLineEdit->isEnabled() || pLineEdit->isReadOnly()) {
        return;
    }

    if (!isDeviceReady(true) || (m_nOffset < 0) || (m_nSize <= 0)) {
        showData(m_nOffset, m_nSize);
        return;
    }

    const qint32 nTypeValue = pLineEdit->property("STYPE").toInt();
    if ((nTypeValue < 0) || (nTypeValue >= __DATAINS_SIZE)) {
        return;
    }

    const DATAINS dataType = static_cast<DATAINS>(nTypeValue);
    const bool bIsBigEndian = (static_cast<XBinary::ENDIAN>(ui->comboBoxEndianness->currentData(Qt::UserRole).toUInt()) == XBinary::ENDIAN_BIG);
    const QByteArray baWriteData = createWriteData(dataType, varValue, bIsBigEndian);

    if (baWriteData.isEmpty() || (baWriteData.size() > m_nSize)) {
        showData(m_nOffset, m_nSize);
        ui->labelStatus->setText(tr("The value cannot be written to this selection."));
        return;
    }

    QPointer<QIODevice> pDevice = m_pDevice;
    if (!pDevice || (m_nOffset > pDevice->size()) || (baWriteData.size() > pDevice->size() - m_nOffset)) {
        showData(m_nOffset, m_nSize);
        ui->labelStatus->setText(tr("The selected write range is no longer available."));
        return;
    }

    const qint64 nOriginalPosition = pDevice->pos();
    if (nOriginalPosition < 0) {
        showData(m_nOffset, m_nSize);
        ui->labelStatus->setText(tr("The device position is unavailable, so the value was not changed."));
        return;
    }
    XOptions *pOptions = getGlobalOptions();
    QString sOutcome;
    qint64 nChangedSize = 0;
    qint64 nChangedOffset = m_nOffset;

    {
        QScopedValueRollback<bool> syncGuard(m_bSync, true);

        QIODevice *pBackupDevice = XBinary::getBackupDevice(pDevice);

        if (pOptions && pOptions->isSaveBackup() && !XBinary::isBackupPresent(pBackupDevice) && !XBinary::saveBackup(pBackupDevice)) {
            sOutcome = tr("A backup could not be created, so the value was not changed.");
        } else {
            QByteArray baOriginal(baWriteData.size(), 0);
            const qint64 nRead = readExactAt(pDevice, m_nOffset, baOriginal.data(), baOriginal.size());

            if (nRead != baOriginal.size()) {
                sOutcome =
                    tr("The original value could not be read, so no change was "
                       "attempted.");
            } else {
                const qint64 nWritten = writeExactAt(pDevice, m_nOffset, baWriteData.constData(), baWriteData.size());

                if (nWritten == baWriteData.size()) {
                    nChangedSize = nWritten;
                    sOutcome = tr("The value was updated.");
                } else if (nWritten == 0) {
                    sOutcome = tr("The value could not be written.");
                } else {
                    const qint64 nRestored = writeExactAt(pDevice, m_nOffset, baOriginal.constData(), nWritten);

                    if (nRestored == nWritten) {
                        sOutcome =
                            tr("The value could not be written completely. The "
                               "original bytes were restored.");
                    } else {
                        QByteArray baAfter(nWritten, 0);
                        const qint64 nAfterRead = readExactAt(pDevice, m_nOffset, baAfter.data(), baAfter.size());
                        qint64 nFirstChanged = -1;
                        qint64 nLastChanged = -1;

                        if (nAfterRead == baAfter.size()) {
                            for (qint64 i = 0; i < nWritten; ++i) {
                                if (baAfter.at(static_cast<qint32>(i)) != baOriginal.at(static_cast<qint32>(i))) {
                                    if (nFirstChanged == -1) nFirstChanged = i;
                                    nLastChanged = i;
                                }
                            }
                        }

                        if (nFirstChanged != -1) {
                            nChangedOffset += nFirstChanged;
                            nChangedSize = nLastChanged - nFirstChanged + 1;
                        } else if (nAfterRead != baAfter.size()) {
                            nFirstChanged = qMin(nRestored, nWritten);
                            nChangedOffset += nFirstChanged;
                            nChangedSize = nWritten - nFirstChanged;
                        }

                        if (nChangedSize > 0) {
                            sOutcome =
                                tr("The write and its rollback were incomplete. The "
                                   "display was refreshed from the device.");
                        } else {
                            sOutcome =
                                tr("The value could not be written completely, but "
                                   "the original bytes remain intact.");
                        }
                    }
                }
            }
        }
    }

    showData(m_nOffset, m_nSize);

    const bool bPositionRestored = pDevice && (nOriginalPosition >= 0) && pDevice->seek(nOriginalPosition);
    if (!bPositionRestored) {
        sOutcome += QLatin1Char(' ') + tr("The device position could not be restored.");
    }
    ui->labelStatus->setText(sOutcome);

    if (nChangedSize > 0) {
        emit dataChanged(nChangedOffset, nChangedSize);
    }
}

void DialogDataInspector::on_pushButtonClose_clicked()
{
    reject();
}

void DialogDataInspector::on_checkBoxReadonly_stateChanged(int nArg)
{
    Q_UNUSED(nArg)

    setReadonly(ui->checkBoxReadonly->isChecked());
}

void DialogDataInspector::on_comboBoxEndianness_currentIndexChanged(int nIndex)
{
    Q_UNUSED(nIndex)

    showData(m_nOffset, m_nSize);
}

void DialogDataInspector::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
}
