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
 * IMPDATAINS, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "dialogdatainspector.h"

#include "ui_dialogdatainspector.h"

DialogDataInspector::DialogDataInspector(QWidget *pParent, QIODevice *pDevice, qint64 nOffset, qint64 nSize)
    : XShortcutsDialog(pParent, false), ui(new Ui::DialogDataInspector)
{
    ui->setupUi(this);

    ui->checkBoxReadonly->setToolTip(tr("Readonly"));
    ui->comboBoxEndianness->setToolTip(tr("Endianness"));
    ui->lineEditOffset->setToolTip(tr("Offset"));
    ui->lineEditSize->setToolTip(tr("Size"));

    m_pDevice = pDevice;
    m_nOffset = nOffset;
    m_nSize = nSize;
    m_bSync = false;

    memset(m_lineEdit, 0, sizeof m_lineEdit);

    ui->tableWidgetDataInspector->setColumnCount(2);
    ui->tableWidgetDataInspector->setRowCount(__DATAINS_SIZE);

    QStringList slHeader;
    slHeader.append(tr("Name"));
    slHeader.append(tr("Value"));

    ui->tableWidgetDataInspector->setHorizontalHeaderLabels(slHeader);
    ui->tableWidgetDataInspector->horizontalHeader()->setVisible(true);

    ui->tableWidgetDataInspector->setColumnWidth(0, 100);  // TODO consts

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
    // addRecord("utf8", DATAINS_UTF8);

    showData(nOffset, nSize);

    ui->checkBoxReadonly->setEnabled(pDevice->isWritable());
    ui->checkBoxReadonly->setChecked(true);

    setReadonly(true);

    XFormats::setEndiannessComboBox(ui->comboBoxEndianness, XBinary::ENDIAN_LITTLE);

    ui->lineEditOffset->setReadOnly(true);
    ui->lineEditSize->setReadOnly(true);
}

DialogDataInspector::~DialogDataInspector()
{
    delete ui;
}

void DialogDataInspector::adjustView()
{
}

void DialogDataInspector::currentLocationChangedSlot(quint64 nLocation, qint32 nLocationType, qint64 nSize)
{
    if (nLocationType == XBinary::LT_OFFSET) {
        showData(nLocation, nSize);
    }
}

void DialogDataInspector::dataChangedSlot(qint64 nOffset, qint64 nSize)
{
    showData(nOffset, nSize);
}

void DialogDataInspector::addRecord(const QString &sTitle, const DATAINS &datains)
{
    QTableWidgetItem *pItemName = new QTableWidgetItem;
    pItemName->setText(sTitle);
    ui->tableWidgetDataInspector->setItem(datains, 0, pItemName);

    m_lineEdit[datains] = new XLineEditHEX(this);
    m_lineEdit[datains]->setProperty("STYPE", datains);

    connect(m_lineEdit[datains], SIGNAL(valueChanged(QVariant)), this, SLOT(valueChangedSlot(QVariant)));

    ui->tableWidgetDataInspector->setCellWidget(datains, 1, m_lineEdit[datains]);
}

void DialogDataInspector::blockSignals(bool bState)
{
    for (qint32 i = 0; i < __DATAINS_SIZE; i++) {
        m_lineEdit[i]->blockSignals(bState);
    }
}

void DialogDataInspector::setReadonly(bool bState)
{
    for (qint32 i = 0; i < __DATAINS_SIZE; i++) {
        m_lineEdit[i]->setReadOnly(bState);
    }
}

void DialogDataInspector::showData(qint64 nOffset, qint64 nSize)
{
    m_nOffset = nOffset;
    m_nSize = nSize;

    ui->lineEditOffset->setValue32_64(m_nOffset);
    ui->lineEditSize->setValue32_64(m_nSize);

    blockSignals(true);

    bool bIsBigEndian = ((XBinary::ENDIAN)(ui->comboBoxEndianness->currentData(Qt::UserRole).toUInt()) == XBinary::ENDIAN_BIG);

    XBinary binary(m_pDevice);

    if (!m_lineEdit[DATAINS_BYTE]->isFocused() || !m_bSync) m_lineEdit[DATAINS_BYTE]->setValue_uint8(binary.read_uint8(nOffset), XLineEditHEX::_MODE_HEX);
    if (!m_lineEdit[DATAINS_UINT8]->isFocused() || !m_bSync) m_lineEdit[DATAINS_UINT8]->setValue_uint8(binary.read_uint8(nOffset), XLineEditHEX::_MODE_DEC);
    if (!m_lineEdit[DATAINS_INT8]->isFocused() || !m_bSync) m_lineEdit[DATAINS_INT8]->setValue_int8(binary.read_int8(nOffset), XLineEditHEX::_MODE_SIGN_DEC);
    if (!m_lineEdit[DATAINS_ANSI]->isFocused() || !m_bSync) m_lineEdit[DATAINS_ANSI]->setValue_String(binary.read_ansiString(nOffset, nSize), nSize);
    if (!m_lineEdit[DATAINS_BINARY]->isFocused() || !m_bSync) m_lineEdit[DATAINS_BINARY]->setValue_uint8(binary.read_uint8(nOffset), XLineEditHEX::_MODE_BIN);

    if (nSize >= 2) {
        enableRow(DATAINS_WORD, true);
        enableRow(DATAINS_UINT16, true);
        enableRow(DATAINS_INT16, true);
        enableRow(DATAINS_UNICODE, true);

        if (!m_lineEdit[DATAINS_WORD]->isFocused() || !m_bSync)
            m_lineEdit[DATAINS_WORD]->setValue_uint16(binary.read_uint16(nOffset, bIsBigEndian), XLineEditHEX::_MODE_HEX);
        if (!m_lineEdit[DATAINS_UINT16]->isFocused() || !m_bSync)
            m_lineEdit[DATAINS_UINT16]->setValue_uint16(binary.read_uint16(nOffset, bIsBigEndian), XLineEditHEX::_MODE_DEC);
        if (!m_lineEdit[DATAINS_INT16]->isFocused() || !m_bSync)
            m_lineEdit[DATAINS_INT16]->setValue_int16(binary.read_int16(nOffset, bIsBigEndian), XLineEditHEX::_MODE_SIGN_DEC);
        if (!m_lineEdit[DATAINS_UNICODE]->isFocused() || !m_bSync)
            m_lineEdit[DATAINS_UNICODE]->setValue_String(binary.read_unicodeString(nOffset, nSize / 2, bIsBigEndian), nSize / 2);
    } else {
        enableRow(DATAINS_WORD, false);
        enableRow(DATAINS_UINT16, false);
        enableRow(DATAINS_INT16, false);
        enableRow(DATAINS_UNICODE, false);
    }

    if (nSize >= 4) {
        enableRow(DATAINS_DWORD, true);
        enableRow(DATAINS_UINT32, true);
        enableRow(DATAINS_INT32, true);

        if (!m_lineEdit[DATAINS_DWORD]->isFocused() || !m_bSync)
            m_lineEdit[DATAINS_DWORD]->setValue_uint32(binary.read_uint32(nOffset, bIsBigEndian), XLineEditHEX::_MODE_HEX);
        if (!m_lineEdit[DATAINS_UINT32]->isFocused() || !m_bSync)
            m_lineEdit[DATAINS_UINT32]->setValue_uint32(binary.read_uint32(nOffset, bIsBigEndian), XLineEditHEX::_MODE_DEC);
        if (!m_lineEdit[DATAINS_INT32]->isFocused() || !m_bSync)
            m_lineEdit[DATAINS_INT32]->setValue_int32(binary.read_int32(nOffset, bIsBigEndian), XLineEditHEX::_MODE_SIGN_DEC);
    } else {
        enableRow(DATAINS_DWORD, false);
        enableRow(DATAINS_UINT32, false);
        enableRow(DATAINS_INT32, false);
    }

    if (nSize >= 8) {
        enableRow(DATAINS_QWORD, true);
        enableRow(DATAINS_UINT64, true);
        enableRow(DATAINS_INT64, true);
        if (!m_lineEdit[DATAINS_QWORD]->isFocused() || !m_bSync)
            m_lineEdit[DATAINS_QWORD]->setValue_uint64(binary.read_uint64(nOffset, bIsBigEndian), XLineEditHEX::_MODE_HEX);
        if (!m_lineEdit[DATAINS_UINT64]->isFocused() || !m_bSync)
            m_lineEdit[DATAINS_UINT64]->setValue_uint64(binary.read_uint64(nOffset, bIsBigEndian), XLineEditHEX::_MODE_DEC);
        if (!m_lineEdit[DATAINS_INT64]->isFocused() || !m_bSync)
            m_lineEdit[DATAINS_INT64]->setValue_int64(binary.read_int64(nOffset, bIsBigEndian), XLineEditHEX::_MODE_SIGN_DEC);
    } else {
        enableRow(DATAINS_QWORD, false);
        enableRow(DATAINS_UINT64, false);
        enableRow(DATAINS_INT64, false);
    }

    // if (!g_lineEdit[DATAINS_UTF8]->isFocused() || !m_bSync) g_lineEdit[DATAINS_UTF8]->setValue_String(binary.read_utf8String(nOffset, nSize), nSize);

    blockSignals(false);
}

void DialogDataInspector::enableRow(qint32 nRow, bool bState)
{
    if (bState) {
        ui->tableWidgetDataInspector->showRow(nRow);
        m_lineEdit[nRow]->setEnabled(true);
    } else {
        m_lineEdit[nRow]->setEnabled(false);
        m_lineEdit[nRow]->clear();
    }
}

void DialogDataInspector::valueChangedSlot(QVariant varValue)
{
    XLineEditHEX *pLineEdit = qobject_cast<XLineEditHEX *>(sender());

    if (pLineEdit->isEnabled()) {
        DATAINS nType = (DATAINS)(pLineEdit->property("STYPE").toInt());

        bool bIsBigEndian = ((XBinary::ENDIAN)(ui->comboBoxEndianness->currentData(Qt::UserRole).toUInt()) == XBinary::ENDIAN_BIG);

        m_bSync = true;

        bool bSuccess = true;

        if ((getGlobalOptions()->isSaveBackup()) && (!XBinary::isBackupPresent(m_pDevice))) {
            bSuccess = XBinary::saveBackup(m_pDevice);
        }

        if (bSuccess) {
            if (m_pDevice->isWritable()) {
                XBinary binary(m_pDevice);

                if (nType == DATAINS_BYTE) binary.write_uint8(m_nOffset, (quint8)varValue.toULongLong());
                else if (nType == DATAINS_WORD) binary.write_uint16(m_nOffset, (quint16)varValue.toULongLong(), bIsBigEndian);
                else if (nType == DATAINS_DWORD) binary.write_uint32(m_nOffset, (quint32)varValue.toULongLong(), bIsBigEndian);
                else if (nType == DATAINS_QWORD) binary.write_uint64(m_nOffset, (quint64)varValue.toULongLong(), bIsBigEndian);
                else if (nType == DATAINS_UINT8) binary.write_uint8(m_nOffset, (quint8)varValue.toULongLong());
                else if (nType == DATAINS_INT8) binary.write_int8(m_nOffset, (qint8)varValue.toULongLong());
                else if (nType == DATAINS_UINT16) binary.write_uint16(m_nOffset, (quint16)varValue.toULongLong(), bIsBigEndian);
                else if (nType == DATAINS_INT16) binary.write_int16(m_nOffset, (qint16)varValue.toULongLong(), bIsBigEndian);
                else if (nType == DATAINS_UINT32) binary.write_uint32(m_nOffset, (quint32)varValue.toULongLong(), bIsBigEndian);
                else if (nType == DATAINS_INT32) binary.write_int32(m_nOffset, (qint32)varValue.toULongLong(), bIsBigEndian);
                else if (nType == DATAINS_UINT64) binary.write_uint64(m_nOffset, (quint64)varValue.toULongLong(), bIsBigEndian);
                else if (nType == DATAINS_INT64) binary.write_int64(m_nOffset, (qint64)varValue.toULongLong(), bIsBigEndian);
                else if (nType == DATAINS_ANSI) binary.write_ansiString(m_nOffset, varValue.toString(), m_nSize);
                else if (nType == DATAINS_UNICODE) binary.write_unicodeString(m_nOffset, varValue.toString(), m_nSize / 2, bIsBigEndian);
                else if (nType == DATAINS_BINARY) binary.write_uint8(m_nOffset, (quint8)varValue.toULongLong());
                // else if (nType == DATAINS_UTF8) binary.write_utf8String(m_nOffset, m_nSize, varValue.toString());

                // selectionChangedSlot(m_nOffset, m_nSize);
                showData(m_nOffset, m_nSize);

                emit dataChanged(m_nOffset, m_nSize);
            }
        }

        m_bSync = false;
    }
}

void DialogDataInspector::on_pushButtonClose_clicked()
{
    this->close();
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
