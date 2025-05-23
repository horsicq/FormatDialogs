/* Copyright (c) 2019-2025 hors<horsicq@gmail.com>
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

#include "ui_dialoghexsignature.h"

DialogHexSignature::DialogHexSignature(QWidget *pParent, QIODevice *pDevice, qint64 nOffset, qint64 nSize)
    : XShortcutsDialog(pParent, false), ui(new Ui::DialogHexSignature)
{
    ui->setupUi(this);

    g_pDevice = pDevice;

    ui->textEditSignature->setWordWrapMode(QTextOption::WrapAnywhere);

    for (qint32 i = 0; i < G_N_MAX_BYTES; i++) {
        g_pushButton[i] = new QPushButton;
        g_pushButton[i]->setMaximumWidth(30);   // TODO Consts
        g_pushButton[i]->setMaximumHeight(20);  // TODO Consts
        g_pushButton[i]->setCheckable(true);
        g_pushButton[i]->setEnabled(false);

        connect(g_pushButton[i], SIGNAL(toggled(bool)), this, SLOT(reload()));

        if ((i >= 0) && (i < 16))  // TODO Consts
        {
            ui->horizontalLayout0->addWidget(g_pushButton[i]);
        } else if ((i >= 16) && (i < 32)) {
            ui->horizontalLayout1->addWidget(g_pushButton[i]);
        } else if ((i >= 32) && (i < 48)) {
            ui->horizontalLayout2->addWidget(g_pushButton[i]);
        } else if ((i >= 48) && (i < 64)) {
            ui->horizontalLayout3->addWidget(g_pushButton[i]);
        } else if ((i >= 64) && (i < 80)) {
            ui->horizontalLayout4->addWidget(g_pushButton[i]);
        } else if ((i >= 80) && (i < 96)) {
            ui->horizontalLayout5->addWidget(g_pushButton[i]);
        } else if ((i >= 96) && (i < 112)) {
            ui->horizontalLayout6->addWidget(g_pushButton[i]);
        } else if ((i >= 112) && (i < 128)) {
            ui->horizontalLayout7->addWidget(g_pushButton[i]);
        }
    }

    nSize = qMin(nSize, (qint64)128);

    g_baData = XBinary::read_array(pDevice, nOffset, nSize);

    for (qint32 i = 0; i < nSize; i++) {
        g_pushButton[i]->setText(QString("%1").arg((quint8)(g_baData.data()[i]), 2, 16, QChar('0')).toUpper());
        g_pushButton[i]->setEnabled(true);
    }

    reload();

    XOptions::setMonoFont(ui->textEditSignature);
}

DialogHexSignature::~DialogHexSignature()
{
    delete ui;
}

void DialogHexSignature::adjustView()
{
}

void DialogHexSignature::on_pushButtonOK_clicked()
{
    this->close();
}

void DialogHexSignature::reload()
{
    qint32 _nMax = G_N_MAX_BYTES;
    qint32 nSize = qMin(_nMax, g_baData.size());
    QString sSignature;
    QString sTemp;

    bool bIsSpace = ui->checkBoxSpaces->isChecked();
    bool bIsUpper = ui->checkBoxUpper->isChecked();
    bool bIsANSI = ui->checkBoxANSI->isChecked();
    QString sWildcard = ui->lineEditWildcard->text();

    bool bIsCurrentANSI = false;

    for (qint32 i = 0; i < nSize; i++) {
        bool bIsWildCard = g_pushButton[i]->isChecked();

        quint8 cSymbol = g_baData.data()[i];

        bool bIsSymbolAnsi = false;

        if (bIsANSI) {
            bIsSymbolAnsi = (cSymbol >= 20) && (cSymbol < 0x7F) && (cSymbol != 0x27) && (cSymbol != 0x22) && (!bIsWildCard);
        }

        if (bIsANSI) {
            if (bIsSymbolAnsi != bIsCurrentANSI) {
                sSignature += "'";
            }
        }

        if (bIsWildCard) {
            sTemp = sWildcard + sWildcard;
        } else {
            if (bIsSymbolAnsi) {
                sTemp = (char)cSymbol;
            } else {
                sTemp = QString("%1").arg(cSymbol, 2, 16, QChar('0'));
                if (bIsUpper) {
                    sTemp = sTemp.toUpper();
                }
            }
        }

        sSignature += sTemp;

        if (bIsANSI) {
            if ((i == (nSize - 1)) && bIsSymbolAnsi) {
                sSignature += "'";
            }
        }

        if (bIsANSI) {
            bIsCurrentANSI = bIsSymbolAnsi;
        }

        if ((bIsSpace) && (i != (nSize - 1))) {
            if (!bIsSymbolAnsi) {
                sSignature += " ";
            }
        }
    }

    ui->textEditSignature->setText(sSignature);
}

void DialogHexSignature::on_pushButtonCopy_clicked()
{
    QClipboard *pClipboard = QApplication::clipboard();
    pClipboard->setText(ui->textEditSignature->toPlainText());
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
    SearchValuesWidget::OPTIONS options = {};
    options.fileType = XBinary::FT_UNKNOWN;
    options.valueType = XBinary::VT_SIGNATURE;
    options.endian = XBinary::ENDIAN_UNKNOWN;
    options.varValue = ui->textEditSignature->toPlainText();
    options.bScan = true;

    DialogSearchValues dialogSearchValues(this);
    dialogSearchValues.setGlobal(getShortcuts(), getGlobalOptions());
    dialogSearchValues.setData(g_pDevice, options);

    dialogSearchValues.exec();
}

void DialogHexSignature::on_checkBoxANSI_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)

    reload();
}

void DialogHexSignature::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
}
