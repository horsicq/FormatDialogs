/* Copyright (c) 2017-2024 hors<horsicq@gmail.com>
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
#ifndef DIALOGEDITSTRING_H
#define DIALOGEDITSTRING_H

#include "xshortcutsdialog.h"

#include "xbinary.h"

namespace Ui {
class DialogEditString;
}

class DialogEditString : public XShortcutsDialog {
    Q_OBJECT

public:
    struct DATA_STRUCT {
        qint64 nOffset;
        qint64 nSize;
        XBinary::MS_RECORD_TYPE recordType;
        QString sString;
        bool bIsNullTerminated;
    };

    explicit DialogEditString(QWidget *pParent, QIODevice *pDevice, DATA_STRUCT *pData_struct);
    ~DialogEditString();

    virtual void adjustView()
    {
    }

private slots:
    void on_pushButtonCancel_clicked();
    void on_pushButtonOK_clicked();
    void on_comboBoxType_currentIndexChanged(int nIndex);
    void on_lineEditString_textChanged(const QString &sStrings);
    void on_checkBoxKeepSize_toggled(bool bChecked);
    void on_checkBoxNullTerminated_toggled(bool bChecked);
    void adjust();

protected:
    virtual void registerShortcuts(bool bState)
    {
        Q_UNUSED(bState)
    }

private:
    Ui::DialogEditString *ui;
    QIODevice *g_pDevice;
    DATA_STRUCT *g_pData_struct;
    qint64 g_nSize;
};

#endif  // DIALOGEDITSTRING_H
