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

    _addItem(QString("C"), DTYPE_C);
    //    _addItem("C++", DTYPE_CPP);
    _addItem(QString("Base64"), DTYPE_BASE64);

    ui->listWidgetType->setCurrentRow(0);
}

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
        sResult += QString("const uint8_t data[%1] = {\n").arg(g_nSize);
    } else if (dtype == DTYPE_CPP) {
    }

    if ((dtype == DTYPE_C) || (dtype == DTYPE_CPP)) {
        XBinary binary(g_pDevice);

        for (qint32 i = 0; i < g_nSize; i++) {
            if ((i % 8) == 0) {
                sResult += "    ";
            }

            sResult += "0x" + XBinary::valueToHex(binary.read_uint8(g_nOffset + i));

            if (i != (g_nSize - 1)) {
                sResult += ",";
            }

            if (((i + 1) % 8 == 0) || (i == (g_nSize - 1))) {
                sResult += "\n";
            } else {
                sResult += " ";
            }
        }
    } else if (dtype == DTYPE_BASE64) {
        XBinary binary(g_pDevice);

        QByteArray baArray = binary.read_array(g_nOffset, g_nSize);

        sResult = baArray.toBase64();
    }

    if ((dtype == DTYPE_C) || (dtype == DTYPE_CPP)) {
        sResult += "};";
    }

    // sResult = XBinary::read_array(g_pDevice, g_nOffset, g_nSize).toHex();

    return sResult;
}

void DialogShowData::_addItem(const QString &sName, DTYPE dtype)
{
    QListWidgetItem *pItem = new QListWidgetItem;
    pItem->setText(sName);
    pItem->setData(Qt::UserRole, dtype);

    ui->listWidgetType->addItem(pItem);
}
