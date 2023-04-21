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
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "dialogshowdata.h"

#include "ui_dialogshowdata.h"

DialogShowData::DialogShowData(QWidget *pParent, QIODevice *pDevice, qint64 nOffset, qint64 nSize) : QDialog(pParent), ui(new Ui::DialogShowData)
{
    ui->setupUi(this);

    g_pDevice = pDevice;
    g_nOffset = nOffset;
    g_nSize = qMin(nSize, (qint64)0x10000);

    _addItem("C", DTYPE_C);
    _addItem("C++", DTYPE_CPP);

    ui->listWidgetType->setCurrentRow(0);
}

//const uint8_t data[101] = {
//    0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00,
//    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
//    0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x05, 0x00,
//    0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
//    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
//    0x00, 0x00, 0x00, 0x00, 0x00
//};


DialogShowData::~DialogShowData()
{
    delete ui;
}

void DialogShowData::on_pushButtonOK_clicked()
{
    this->close();
}

void DialogShowData::reload()
{
    if (ui->listWidgetType->currentRow() != -1) {
        DTYPE dtype = (DTYPE)(ui->listWidgetType->currentItem()->data(Qt::UserRole).toUInt());

        QString sData = getDataString(dtype);

        ui->plainTextEditData->setPlainText(sData);
    }
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

QString DialogShowData::getDataString(DTYPE dtype)
{
    Q_UNUSED(dtype)
    // TODO
    QString sResult;

    if (dtype == DTYPE_C) {
        sResult += QString("const uint8_t data[%1] = {").arg(g_nSize);
    } else if (dtype == DTYPE_CPP) {

    }

    // TODO

    if ((dtype == DTYPE_C) ||
        (dtype == DTYPE_CPP)) {
        sResult += "};";
    }

    //sResult = XBinary::read_array(g_pDevice, g_nOffset, g_nSize).toHex();

    return sResult;
}

void DialogShowData::_addItem(QString sName, DTYPE dtype)
{
    QListWidgetItem *pItem = new QListWidgetItem;
    pItem->setText(sName);
    pItem->setData(Qt::UserRole, dtype);

    ui->listWidgetType->addItem(pItem);
}

