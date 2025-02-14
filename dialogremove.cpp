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
#include "dialogremove.h"

#include "ui_dialogremove.h"

DialogRemove::DialogRemove(QWidget *pParent, DATA *pData) : XShortcutsDialog(pParent, false), ui(new Ui::DialogRemove)
{
    ui->setupUi(this);

    g_pData = pData;

    ui->checkBoxHex->setChecked(true);
    ui->lineEditOffset->setValue_int64(pData->nOffset);
    ui->lineEditSize->setValue_int64(pData->nSize);
}

DialogRemove::~DialogRemove()
{
    delete ui;
}

void DialogRemove::adjustView()
{
}

void DialogRemove::on_pushButtonCancel_clicked()
{
    reject();
}

void DialogRemove::on_pushButtonOK_clicked()
{
    bool bValid = false;

    g_pData->nOffset = ui->lineEditOffset->getValue_int64();
    g_pData->nSize = ui->lineEditSize->getValue_int64();

    if ((g_pData->nOffset >= 0) && (g_pData->nSize > 0) && ((g_pData->nOffset + g_pData->nSize) <= g_pData->nMaxSize)) {
        bValid = true;
    }

    if (bValid) {
        accept();
    } else {
        ui->labelStatus->setText(tr("Invalid"));
    }
}

void DialogRemove::on_checkBoxHex_toggled(bool bChecked)
{
    if (bChecked) {
        ui->lineEditOffset->setMode(XLineEditHEX::_MODE_HEX);
        ui->lineEditSize->setMode(XLineEditHEX::_MODE_HEX);
    } else {
        ui->lineEditOffset->setMode(XLineEditHEX::_MODE_DEC);
        ui->lineEditSize->setMode(XLineEditHEX::_MODE_DEC);
    }
}

void DialogRemove::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
}
