/* Copyright (c) 2019-2026 hors<horsicq@gmail.com>
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
#ifndef DIALOGGOTOADDRESS_H
#define DIALOGGOTOADDRESS_H

#include "xbinary.h"
#include "xshortcutsdialog.h"

namespace Ui {
class DialogGoToAddress;
}

class DialogGoToAddress : public XShortcutsDialog {
    Q_OBJECT

public:
    enum TYPE {
        TYPE_VIRTUALADDRESS = 0,
        TYPE_RELVIRTUALADDRESS,
        TYPE_OFFSET,
        TYPE_ADDRESS
    };

    DialogGoToAddress(QWidget *pParent, XBinary::_MEMORY_MAP *pMemoryMap, TYPE type, XADDR nCurrentValue = 0,
                      XADDR nMaximumValue = XADDR_MAX - 1);
    DialogGoToAddress(QWidget *pParent, XADDR nMinValue, XADDR nMaxValue, TYPE type, XADDR nCurrentValue);
    ~DialogGoToAddress() override;

    void adjustView() override;

    Q_DECL_DEPRECATED_X("Use getValue_XADDR() to preserve unsigned 64-bit addresses") qint64 getValue();
    XADDR getValue_XADDR() const;

private:
    void initialize();
    void adjustTitle(TYPE type);
    void updateRangeDescription();
    void updateState();
    bool readValue(XADDR *pValue, QString *pError = nullptr) const;
    bool isValueValid(XADDR nValue) const;
    XADDR effectiveMaximum() const;
    QString formatValue(XADDR nValue) const;

private slots:
    void acceptValue();
    void on_checkBoxHex_toggled(bool bChecked);

protected:
    void registerShortcuts(bool bState) override;

private:
    Ui::DialogGoToAddress *ui;
    TYPE m_type;
    XBinary::_MEMORY_MAP m_memoryMap;
    bool m_bUseMemoryMap;
    bool m_bHasMemoryMap;
    XADDR m_nMinValue;
    XADDR m_nMaxValue;
    XADDR m_nValue;
};

#endif  // DIALOGGOTOADDRESS_H
