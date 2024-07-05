/* Copyright (c) 2023-2024 hors<horsicq@gmail.com>
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

    g_pDevice = pDevice;
    g_nOffset = nOffset;
    g_nSize = nSize;
    g_bSync = false;

    memset(g_lineEdit, 0, sizeof g_lineEdit);

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

void DialogDataInspector::selectionChangedSlot(qint64 nOffset, qint64 nSize)
{
    showData(nOffset, nSize);
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

    g_lineEdit[datains] = new XLineEditHEX(this);
    g_lineEdit[datains]->setProperty("STYPE", datains);

    connect(g_lineEdit[datains], SIGNAL(valueChanged(QVariant)), this, SLOT(valueChangedSlot(QVariant)));

    ui->tableWidgetDataInspector->setCellWidget(datains, 1, g_lineEdit[datains]);
}

void DialogDataInspector::blockSignals(bool bState)
{
    for (qint32 i = 0; i < __DATAINS_SIZE; i++) {
        g_lineEdit[i]->blockSignals(bState);
    }
}

void DialogDataInspector::setReadonly(bool bState)
{
    for (qint32 i = 0; i < __DATAINS_SIZE; i++) {
        g_lineEdit[i]->setReadOnly(bState);
    }
}

void DialogDataInspector::showData(qint64 nOffset, qint64 nSize)
{
    g_nOffset = nOffset;
    g_nSize = nSize;

    ui->lineEditOffset->setValue32_64(g_nOffset);
    ui->lineEditSize->setValue32_64(g_nSize);

    blockSignals(true);

    bool bIsBigEndian = ((XBinary::ENDIAN)(ui->comboBoxEndianness->currentData(Qt::UserRole).toUInt()) == XBinary::ENDIAN_BIG);

    XBinary binary(g_pDevice);

    if (!g_lineEdit[DATAINS_BYTE]->isFocused() || !g_bSync) g_lineEdit[DATAINS_BYTE]->setValue_uint8(binary.read_uint8(nOffset), XLineEditHEX::_MODE_HEX);
    if (!g_lineEdit[DATAINS_UINT8]->isFocused() || !g_bSync) g_lineEdit[DATAINS_UINT8]->setValue_uint8(binary.read_uint8(nOffset), XLineEditHEX::_MODE_DEC);
    if (!g_lineEdit[DATAINS_INT8]->isFocused() || !g_bSync) g_lineEdit[DATAINS_INT8]->setValue_int8(binary.read_int8(nOffset), XLineEditHEX::_MODE_SIGN_DEC);
    if (!g_lineEdit[DATAINS_ANSI]->isFocused() || !g_bSync) g_lineEdit[DATAINS_ANSI]->setValue_String(binary.read_ansiString(nOffset, nSize), nSize);
    if (!g_lineEdit[DATAINS_BINARY]->isFocused() || !g_bSync) g_lineEdit[DATAINS_BINARY]->setValue_uint8(binary.read_uint8(nOffset), XLineEditHEX::_MODE_BIN);

    if (nSize >= 2) {
        ui->tableWidgetDataInspector->showRow(DATAINS_WORD);
        ui->tableWidgetDataInspector->showRow(DATAINS_UINT16);
        ui->tableWidgetDataInspector->showRow(DATAINS_INT16);
        ui->tableWidgetDataInspector->showRow(DATAINS_UNICODE);
        if (!g_lineEdit[DATAINS_WORD]->isFocused() || !g_bSync)
            g_lineEdit[DATAINS_WORD]->setValue_uint16(binary.read_uint16(nOffset, bIsBigEndian), XLineEditHEX::_MODE_HEX);
        if (!g_lineEdit[DATAINS_UINT16]->isFocused() || !g_bSync)
            g_lineEdit[DATAINS_UINT16]->setValue_uint16(binary.read_uint16(nOffset, bIsBigEndian), XLineEditHEX::_MODE_DEC);
        if (!g_lineEdit[DATAINS_INT16]->isFocused() || !g_bSync)
            g_lineEdit[DATAINS_INT16]->setValue_int16(binary.read_int16(nOffset, bIsBigEndian), XLineEditHEX::_MODE_SIGN_DEC);
        if (!g_lineEdit[DATAINS_UNICODE]->isFocused() || !g_bSync)
            g_lineEdit[DATAINS_UNICODE]->setValue_String(binary.read_unicodeString(nOffset, nSize / 2, bIsBigEndian), nSize / 2);
    } else {
        ui->tableWidgetDataInspector->hideRow(DATAINS_WORD);
        ui->tableWidgetDataInspector->hideRow(DATAINS_UINT16);
        ui->tableWidgetDataInspector->hideRow(DATAINS_INT16);
        ui->tableWidgetDataInspector->hideRow(DATAINS_UNICODE);
    }

    if (nSize >= 4) {
        ui->tableWidgetDataInspector->showRow(DATAINS_DWORD);
        ui->tableWidgetDataInspector->showRow(DATAINS_UINT32);
        ui->tableWidgetDataInspector->showRow(DATAINS_INT32);
        if (!g_lineEdit[DATAINS_DWORD]->isFocused() || !g_bSync)
            g_lineEdit[DATAINS_DWORD]->setValue_uint32(binary.read_uint32(nOffset, bIsBigEndian), XLineEditHEX::_MODE_HEX);
        if (!g_lineEdit[DATAINS_UINT32]->isFocused() || !g_bSync)
            g_lineEdit[DATAINS_UINT32]->setValue_uint32(binary.read_uint32(nOffset, bIsBigEndian), XLineEditHEX::_MODE_DEC);
        if (!g_lineEdit[DATAINS_INT32]->isFocused() || !g_bSync)
            g_lineEdit[DATAINS_INT32]->setValue_int32(binary.read_int32(nOffset, bIsBigEndian), XLineEditHEX::_MODE_SIGN_DEC);
    } else {
        ui->tableWidgetDataInspector->hideRow(DATAINS_DWORD);
        ui->tableWidgetDataInspector->hideRow(DATAINS_UINT32);
        ui->tableWidgetDataInspector->hideRow(DATAINS_INT32);
    }

    if (nSize >= 8) {
        ui->tableWidgetDataInspector->showRow(DATAINS_QWORD);
        ui->tableWidgetDataInspector->showRow(DATAINS_UINT64);
        ui->tableWidgetDataInspector->showRow(DATAINS_INT64);
        if (!g_lineEdit[DATAINS_QWORD]->isFocused() || !g_bSync)
            g_lineEdit[DATAINS_QWORD]->setValue_uint64(binary.read_uint64(nOffset, bIsBigEndian), XLineEditHEX::_MODE_HEX);
        if (!g_lineEdit[DATAINS_UINT64]->isFocused() || !g_bSync)
            g_lineEdit[DATAINS_UINT64]->setValue_uint64(binary.read_uint64(nOffset, bIsBigEndian), XLineEditHEX::_MODE_DEC);
        if (!g_lineEdit[DATAINS_INT64]->isFocused() || !g_bSync)
            g_lineEdit[DATAINS_INT64]->setValue_int64(binary.read_int64(nOffset, bIsBigEndian), XLineEditHEX::_MODE_SIGN_DEC);
    } else {
        ui->tableWidgetDataInspector->hideRow(DATAINS_QWORD);
        ui->tableWidgetDataInspector->hideRow(DATAINS_UINT64);
        ui->tableWidgetDataInspector->hideRow(DATAINS_INT64);
    }

    // if (!g_lineEdit[DATAINS_UTF8]->isFocused() || !g_bSync) g_lineEdit[DATAINS_UTF8]->setValue_String(binary.read_utf8String(nOffset, nSize), nSize);

    blockSignals(false);
}

void DialogDataInspector::valueChangedSlot(QVariant varValue)
{
    XLineEditHEX *pLineEdit = qobject_cast<XLineEditHEX *>(sender());

    DATAINS nType = (DATAINS)(pLineEdit->property("STYPE").toInt());

    bool bIsBigEndian = ((XBinary::ENDIAN)(ui->comboBoxEndianness->currentData(Qt::UserRole).toUInt()) == XBinary::ENDIAN_BIG);

    g_bSync = true;

    bool bSuccess = true;

    // TODO Backup device
    if ((getGlobalOptions()->isSaveBackup()) && (!XBinary::isBackupPresent(g_pDevice))) {
        bSuccess = XBinary::saveBackup(g_pDevice);
    }

    if (bSuccess) {
        if (g_pDevice->isWritable()) {
            XBinary binary(g_pDevice);

            if (nType == DATAINS_BYTE) binary.write_uint8(g_nOffset, (quint8)varValue.toULongLong());
            else if (nType == DATAINS_WORD) binary.write_uint16(g_nOffset, (quint16)varValue.toULongLong(), bIsBigEndian);
            else if (nType == DATAINS_DWORD) binary.write_uint32(g_nOffset, (quint32)varValue.toULongLong(), bIsBigEndian);
            else if (nType == DATAINS_QWORD) binary.write_uint64(g_nOffset, (quint64)varValue.toULongLong(), bIsBigEndian);
            else if (nType == DATAINS_UINT8) binary.write_uint8(g_nOffset, (quint8)varValue.toULongLong());
            else if (nType == DATAINS_INT8) binary.write_int8(g_nOffset, (qint8)varValue.toULongLong());
            else if (nType == DATAINS_UINT16) binary.write_uint16(g_nOffset, (quint16)varValue.toULongLong(), bIsBigEndian);
            else if (nType == DATAINS_INT16) binary.write_int16(g_nOffset, (qint16)varValue.toULongLong(), bIsBigEndian);
            else if (nType == DATAINS_UINT32) binary.write_uint32(g_nOffset, (quint32)varValue.toULongLong(), bIsBigEndian);
            else if (nType == DATAINS_INT32) binary.write_int32(g_nOffset, (qint32)varValue.toULongLong(), bIsBigEndian);
            else if (nType == DATAINS_UINT64) binary.write_uint64(g_nOffset, (quint64)varValue.toULongLong(), bIsBigEndian);
            else if (nType == DATAINS_INT64) binary.write_int64(g_nOffset, (qint64)varValue.toULongLong(), bIsBigEndian);
            else if (nType == DATAINS_ANSI) binary.write_ansiString(g_nOffset, varValue.toString(), g_nSize);
            else if (nType == DATAINS_UNICODE) binary.write_unicodeString(g_nOffset, varValue.toString(), g_nSize / 2, bIsBigEndian);
            else if (nType == DATAINS_BINARY) binary.write_uint8(g_nOffset, (quint8)varValue.toULongLong());
            // else if (nType == DATAINS_UTF8) binary.write_utf8String(g_nOffset, g_nSize, varValue.toString());

            // selectionChangedSlot(g_nOffset, g_nSize);
            showData(g_nOffset, g_nSize);

            emit dataChanged(g_nOffset, g_nSize);
        }
    }

    g_bSync = false;
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

    showData(g_nOffset, g_nSize);
}
