/* Copyright (c) 2023-2024 hors<horsicq@gmail.com>
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
#ifndef DIALOGSHOWDATA_H
#define DIALOGSHOWDATA_H

#include "xshortcutsdialog.h"
#include "xbinary.h"
#include "xoptions.h"
#include "xshortcuts.h"

namespace Ui {
class DialogShowData;
}

class DialogShowData : public XShortcutsDialog {
    Q_OBJECT

    enum DTYPE {
        DTYPE_HEX = 0,
        DTYPE_PLAINTEXT,
        DTYPE_C,
        DTYPE_CPP,
        DTYPE_MASM,
        DTYPE_FASM,
        DTYPE_JAVA,
        DTYPE_JAVASCRIPT,
        DTYPE_PYTHON,
        DTYPE_CSHARP,
        DTYPE_VBNET,
        DTYPE_RUST,
        DTYPE_PASCAL,
        DTYPE_LUA,
        DTYPE_GO,
        DTYPE_CRYSTAL,
        DTYPE_SWIFT,
        DTYPE_BASE64,
        // TODO more
    };

public:
    explicit DialogShowData(QWidget *pParent, QIODevice *pDevice, qint64 nOffset, qint64 nSize);
    ~DialogShowData();

    virtual void adjustView() {}

private slots:
    void on_pushButtonOK_clicked();
    void reload();
    void on_pushButtonCopy_clicked();
    void on_listWidgetType_currentRowChanged(int nCurrentRow);
    QString getDataString(DTYPE dtype);
    void _addItem(const QString &sName, DTYPE dtype);
    void on_spinBoxElementsProLine_valueChanged(int nArg);
    void on_checkBoxGroup_toggled(bool bChecked);

protected:
    virtual void registerShortcuts(bool bState) { Q_UNUSED(bState) }

private:
    Ui::DialogShowData *ui;
    QIODevice *g_pDevice;
    qint64 g_nOffset;
    qint64 g_nSize;
};

#endif  // DIALOGSHOWDATA_H
