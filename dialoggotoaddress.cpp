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
#include "dialoggotoaddress.h"

#include "ui_dialoggotoaddress.h"

// mb TODO gesamt constructor
DialogGoToAddress::DialogGoToAddress(QWidget *pParent, XBinary::_MEMORY_MAP *pMemoryMap, TYPE type, XADDR nCurrentValue)
    : XShortcutsDialog(pParent, false), ui(new Ui::DialogGoToAddress)
{
    ui->setupUi(this);

    m_nMinValue = 0;
    m_nMaxValue = 0;
    m_nValue = nCurrentValue;
    m_pMemoryMap = pMemoryMap;
    m_type = type;

    ui->checkBoxHex->setChecked(true);
    ui->lineEditValue->setValue32_64(nCurrentValue);

    adjustTitle(type);
}

DialogGoToAddress::DialogGoToAddress(QWidget *pParent, XADDR nMinValue, XADDR nMaxValue, DialogGoToAddress::TYPE type, XADDR nCurrentValue)
    : XShortcutsDialog(pParent, false), ui(new Ui::DialogGoToAddress)
{
    ui->setupUi(this);

    m_pMemoryMap = nullptr;
    m_nMinValue = nMinValue;
    m_nMaxValue = nMaxValue;
    m_type = type;
    m_nValue = nCurrentValue;

    ui->checkBoxHex->setChecked(true);
    ui->lineEditValue->setValue32_64(nCurrentValue);

    adjustTitle(type);
}

DialogGoToAddress::~DialogGoToAddress()
{
    delete ui;
}

void DialogGoToAddress::adjustView()
{
}

qint64 DialogGoToAddress::getValue()
{
    return m_nValue;
}

void DialogGoToAddress::adjustTitle(DialogGoToAddress::TYPE type)
{
    QString sTitle = "";
    QString sValue = "";

    if (type == TYPE_VIRTUALADDRESS) {
        sTitle = tr("Virtual address");
        sValue = tr("Value");
    } else if (type == TYPE_OFFSET) {
        sTitle = tr("Offset");
        sValue = tr("Value");
    } else if (type == TYPE_RELVIRTUALADDRESS) {
        sTitle = tr("Relative virtual address");
        sValue = tr("Value");
    } else if (type == TYPE_ADDRESS) {
        sTitle = tr("Address");
        sValue = tr("Value");
    }

    setWindowTitle(sTitle);
    ui->groupBoxValue->setTitle(sValue);
}

void DialogGoToAddress::on_pushButtonCancel_clicked()
{
    reject();
}

void DialogGoToAddress::on_pushButtonOK_clicked()
{
    XADDR nValue = (XADDR)ui->lineEditValue->getValue_uint64();

    bool bValid = false;

    if (m_pMemoryMap) {
        if ((m_type == TYPE_VIRTUALADDRESS) || (m_type == TYPE_ADDRESS)) {
            bValid = XBinary::isAddressValid(m_pMemoryMap, nValue) && XBinary::isAddressPhysical(m_pMemoryMap, nValue);
        } else if (m_type == TYPE_OFFSET) {
            bValid = XBinary::isOffsetValid(m_pMemoryMap, nValue);
        } else if (m_type == TYPE_RELVIRTUALADDRESS) {
            bValid = XBinary::isRelAddressValid(m_pMemoryMap, nValue) && XBinary::isRelAddressPhysical(m_pMemoryMap, nValue);
        }
    } else {
        if ((nValue >= m_nMinValue) && (nValue <= m_nMaxValue)) {
            bValid = true;
        }
    }

    if (bValid) {
        this->m_nValue = nValue;
        accept();
    } else {
        ui->labelStatus->setText(tr("Invalid"));
    }
}

void DialogGoToAddress::on_checkBoxHex_toggled(bool bChecked)
{
    if (bChecked) {
        ui->lineEditValue->setMode(XLineEditHEX::_MODE_HEX);
    } else {
        ui->lineEditValue->setMode(XLineEditHEX::_MODE_DEC);
    }
    //    if (!nValue) {
    //        ui->lineEditValue->clear();
    //    }
}

void DialogGoToAddress::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
}
