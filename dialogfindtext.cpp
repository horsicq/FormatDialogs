/* Copyright (c) 2022-2025 hors<horsicq@gmail.com>
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
#include "dialogfindtext.h"

#include "ui_dialogfindtext.h"

DialogFindText::DialogFindText(QWidget *pParent) : XShortcutsDialog(pParent, false), ui(new Ui::DialogFindText)
{
    ui->setupUi(this);

    g_pData = nullptr;
}

DialogFindText::~DialogFindText()
{
    delete ui;
}

void DialogFindText::adjustView()
{
}

void DialogFindText::setData(DATA *pData)
{
    g_pData = pData;

    ui->lineEditText->setText(pData->sText);
    ui->checkBoxMatchCase->setChecked(pData->bIsMatchCase);
}

void DialogFindText::on_pushButtonCancel_clicked()
{
    reject();
}

void DialogFindText::on_pushButtonOK_clicked()
{
    g_pData->sText = ui->lineEditText->text();
    g_pData->bIsMatchCase = ui->checkBoxMatchCase->isChecked();

    accept();
}

void DialogFindText::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
}
