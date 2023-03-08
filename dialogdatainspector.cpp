/* Copyright (c) 2023 hors<horsicq@gmail.com>
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

DialogDataInspector::DialogDataInspector(QWidget *pParent, QIODevice *pDevice) : XShortcutsDialog(pParent), ui(new Ui::DialogDataInspector)
{
    ui->setupUi(this);

    g_pDevice = pDevice;
    g_nOffset = 0;
    g_nSize = 0;
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

    addValue("BYTE", DATAINS_BYTE);
    addValue("WORD", DATAINS_WORD);
    addValue("DWORD", DATAINS_DWORD);
    addValue("QWORD", DATAINS_QWORD);
    addValue("uint8", DATAINS_UINT8);
    addValue("int8", DATAINS_INT8);
    addValue("uint16", DATAINS_UINT16);
    addValue("int16", DATAINS_INT16);
    addValue("uint32", DATAINS_UINT32);
    addValue("int32", DATAINS_INT32);
    addValue("uint64", DATAINS_UINT64);
    addValue("int64", DATAINS_INT64);
}

DialogDataInspector::~DialogDataInspector()
{
    delete ui;
}

void DialogDataInspector::selectionChangedSlot(qint64 nOffset, qint64 nSize)
{
    g_nOffset = nOffset;
    g_nSize = nSize;

    // TODO block signals

    XBinary binary(g_pDevice);

    if (!g_lineEdit[DATAINS_BYTE]->isFocused() || !g_bSync) g_lineEdit[DATAINS_BYTE]->setValue(binary.read_uint8(nOffset));
    if (!g_lineEdit[DATAINS_WORD]->isFocused() || !g_bSync) g_lineEdit[DATAINS_WORD]->setValue(binary.read_uint16(nOffset));
    if (!g_lineEdit[DATAINS_DWORD]->isFocused() || !g_bSync) g_lineEdit[DATAINS_DWORD]->setValue(binary.read_uint32(nOffset));
    if (!g_lineEdit[DATAINS_QWORD]->isFocused() || !g_bSync) g_lineEdit[DATAINS_QWORD]->setValue(binary.read_uint64(nOffset));
    if (!g_lineEdit[DATAINS_UINT8]->isFocused() || !g_bSync) g_lineEdit[DATAINS_UINT8]->setValue(binary.read_uint8(nOffset), HEXValidator::MODE_DEC);
    if (!g_lineEdit[DATAINS_INT8]->isFocused() || !g_bSync) g_lineEdit[DATAINS_INT8]->setValue(binary.read_int8(nOffset), HEXValidator::MODE_SIGN_DEC);
    if (!g_lineEdit[DATAINS_UINT16]->isFocused() || !g_bSync) g_lineEdit[DATAINS_UINT16]->setValue(binary.read_uint16(nOffset), HEXValidator::MODE_DEC);
    if (!g_lineEdit[DATAINS_INT16]->isFocused() || !g_bSync) g_lineEdit[DATAINS_INT16]->setValue(binary.read_int16(nOffset), HEXValidator::MODE_SIGN_DEC);
    if (!g_lineEdit[DATAINS_UINT32]->isFocused() || !g_bSync) g_lineEdit[DATAINS_UINT32]->setValue(binary.read_uint32(nOffset), HEXValidator::MODE_DEC);
    if (!g_lineEdit[DATAINS_INT32]->isFocused() || !g_bSync) g_lineEdit[DATAINS_INT32]->setValue(binary.read_int32(nOffset), HEXValidator::MODE_SIGN_DEC);
    if (!g_lineEdit[DATAINS_UINT64]->isFocused() || !g_bSync) g_lineEdit[DATAINS_UINT64]->setValue(binary.read_uint64(nOffset), HEXValidator::MODE_DEC);
    if (!g_lineEdit[DATAINS_INT64]->isFocused() || !g_bSync) g_lineEdit[DATAINS_INT64]->setValue(binary.read_int64(nOffset), HEXValidator::MODE_SIGN_DEC);

    // TODO
}

void DialogDataInspector::addValue(QString sTitle, DATAINS datains)
{
    QTableWidgetItem *pItemName = new QTableWidgetItem;
    pItemName->setText(sTitle);
    ui->tableWidgetDataInspector->setItem(datains, 0, pItemName);

    g_lineEdit[datains] = new XLineEditHEX(this);
    g_lineEdit[datains]->setProperty("STYPE", datains);

    connect(g_lineEdit[datains], SIGNAL(valueChanged(QVariant)), this, SLOT(valueChangedSlot(QVariant)));

    ui->tableWidgetDataInspector->setCellWidget(datains, 1, g_lineEdit[datains]);
    // TODO Strings
}

void DialogDataInspector::valueChangedSlot(QVariant varValue)
{
    XLineEditHEX *pLineEdit = qobject_cast<XLineEditHEX *>(sender());

    DATAINS nType = (DATAINS)(pLineEdit->property("STYPE").toInt());

    g_bSync = true;

    bool bSuccess = true;

    // TODO Backup device
    if ((getGlobalOptions()->isSaveBackup()) && (!XBinary::isBackupPresent(g_pDevice))) {
        bSuccess = XBinary::saveBackup(g_pDevice);
    }

    if (bSuccess) {
        if (g_pDevice->isWritable()) {
            XBinary binary(g_pDevice);

            if (nType == DATAINS_BYTE)
                binary.write_uint8(g_nOffset, (quint8)varValue.toULongLong());
            else if (nType == DATAINS_WORD)
                binary.write_uint16(g_nOffset, (quint16)varValue.toULongLong());
            else if (nType == DATAINS_DWORD)
                binary.write_uint32(g_nOffset, (quint32)varValue.toULongLong());
            else if (nType == DATAINS_QWORD)
                binary.write_uint64(g_nOffset, (quint64)varValue.toULongLong());
            else if (nType == DATAINS_UINT8)
                binary.write_uint8(g_nOffset, (quint8)varValue.toULongLong());
            else if (nType == DATAINS_INT8)
                binary.write_int8(g_nOffset, (qint8)varValue.toULongLong());
            else if (nType == DATAINS_UINT16)
                binary.write_uint16(g_nOffset, (quint16)varValue.toULongLong());
            else if (nType == DATAINS_INT16)
                binary.write_int16(g_nOffset, (qint16)varValue.toULongLong());
            else if (nType == DATAINS_UINT32)
                binary.write_uint32(g_nOffset, (quint32)varValue.toULongLong());
            else if (nType == DATAINS_INT32)
                binary.write_int32(g_nOffset, (qint32)varValue.toULongLong());
            else if (nType == DATAINS_UINT64)
                binary.write_uint64(g_nOffset, (quint64)varValue.toULongLong());
            else if (nType == DATAINS_INT64)
                binary.write_int64(g_nOffset, (qint64)varValue.toULongLong());

            selectionChangedSlot(g_nOffset, g_nSize);

            emit dataChanged(g_nOffset, g_nSize);
        }
    }

    g_bSync = false;
}

void DialogDataInspector::on_pushButtonClose_clicked()
{
    this->close();
}