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
    _addItem(QString("C++"), DTYPE_CPP);
    _addItem(QString("Java"), DTYPE_JAVA);
    _addItem(QString("JavaScript"), DTYPE_JAVASCRIPT);
    _addItem(QString("Python"), DTYPE_PYTHON);
    _addItem(QString("C#"), DTYPE_CSHARP);
    _addItem(QString("VB.NET"), DTYPE_VBNET);
    _addItem(QString("Rust"), DTYPE_RUST);
    _addItem(QString("Pascal"), DTYPE_PASCAL);
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
        sResult += QString("constexpr std::array<uint8_t, %1> data = {\n").arg(g_nSize);
    } else if (dtype == DTYPE_JAVA) {
        sResult += QString("final byte[] data = {\n");
    } else if (dtype == DTYPE_JAVASCRIPT) {
        sResult += QString("const data = new Uint8Array([\n");
    } else if (dtype == DTYPE_PYTHON) {
        sResult += QString("data = bytes([\n");
    } else if (dtype == DTYPE_CSHARP) {
        sResult += QString("const byte[] data = {\n");
    } else if (dtype == DTYPE_VBNET) {
        sResult += QString("Dim data As Byte(%1) {\n").arg(g_nSize);
    } else if (dtype == DTYPE_RUST) {
        sResult += QString("let data: [u8; 0x%1] = [\n").arg(g_nSize, 0, 16);
    } else if (dtype == DTYPE_PASCAL) {
        sResult += QString("data: array[0..%1] of Byte = (\n").arg(g_nSize);
    }

    if ((dtype == DTYPE_C) || (dtype == DTYPE_CPP) || (dtype == DTYPE_CSHARP) || (dtype == DTYPE_JAVA) || (dtype == DTYPE_VBNET) || (dtype == DTYPE_RUST) ||
        (dtype == DTYPE_PYTHON) || (dtype == DTYPE_JAVASCRIPT) || (dtype == DTYPE_PASCAL)) {
        XBinary binary(g_pDevice);

        for (qint32 i = 0; i < g_nSize; i++) {
            if ((i % 8) == 0) {
                sResult += "    ";
            }

            if ((dtype == DTYPE_C) || (dtype == DTYPE_CPP) || (dtype == DTYPE_CSHARP) || (dtype == DTYPE_JAVA) || (dtype == DTYPE_RUST) || (dtype == DTYPE_PYTHON) ||
                (dtype == DTYPE_JAVASCRIPT)) {
                sResult += "0x" + XBinary::valueToHex(binary.read_uint8(g_nOffset + i)).toUpper();
            } else if (dtype == DTYPE_VBNET) {
                sResult += "&H" + XBinary::valueToHex(binary.read_uint8(g_nOffset + i)).toUpper();
            } else if (dtype == DTYPE_PASCAL) {
                sResult += "$" + XBinary::valueToHex(binary.read_uint8(g_nOffset + i)).toUpper();
            }

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

    if ((dtype == DTYPE_C) || (dtype == DTYPE_CPP) || (dtype == DTYPE_CSHARP) || (dtype == DTYPE_JAVA) || (dtype == DTYPE_VBNET)) {
        sResult += "};";
    } else if (dtype == DTYPE_RUST) {
        sResult += "];";
    } else if (dtype == DTYPE_PYTHON) {
        sResult += "])";
    } else if (dtype == DTYPE_JAVASCRIPT) {
        sResult += "]);";
    } else if (dtype == DTYPE_PASCAL) {
        sResult += ")";
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
