/* Copyright (c) 2023-2025 hors<horsicq@gmail.com>
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
#ifndef DIALOGDATAINSPECTOR_H
#define DIALOGDATAINSPECTOR_H

#include "xshortcutsdialog.h"
#include "xlineedithex.h"
#include "xformats.h"

namespace Ui {
class DialogDataInspector;
}

class DialogDataInspector : public XShortcutsDialog {
    Q_OBJECT

    enum DATAINS {
        //        DATAINS_BINARY=0,
        DATAINS_BYTE,
        DATAINS_WORD,
        DATAINS_DWORD,
        DATAINS_QWORD,
        DATAINS_UINT8,
        DATAINS_INT8,
        DATAINS_UINT16,
        DATAINS_INT16,
        DATAINS_UINT32,
        DATAINS_INT32,
        DATAINS_UINT64,
        DATAINS_INT64,
        DATAINS_ANSI,
        DATAINS_UNICODE,
        // DATAINS_UTF8, // TODO
        DATAINS_BINARY,
        __DATAINS_SIZE
        // TODO FLOAT16
        // TODO FLOAT32
        // TODO FLOAT64
    };

public:
    explicit DialogDataInspector(QWidget *pParent, QIODevice *pDevice, qint64 nOffset, qint64 nSize);
    ~DialogDataInspector();

    virtual void adjustView();

public slots:
    void currentLocationChangedSlot(quint64 nLocation, qint32 nLocationType, qint64 nSize);
    void dataChangedSlot(qint64 nOffset, qint64 nSize);

private:
    void addRecord(const QString &sTitle, const DATAINS &datains);
    void blockSignals(bool bState);
    void setReadonly(bool bState);
    void showData(qint64 nOffset, qint64 nSize);
    void enableRow(qint32 nRow, bool bState);

private slots:
    void valueChangedSlot(QVariant varValue);
    void on_pushButtonClose_clicked();
    void on_checkBoxReadonly_stateChanged(int nArg);
    void on_comboBoxEndianness_currentIndexChanged(int nIndex);

protected:
    virtual void registerShortcuts(bool bState);

private:
    Ui::DialogDataInspector *ui;
    XLineEditHEX *m_lineEdit[__DATAINS_SIZE];
    QIODevice *m_pDevice;
    qint64 g_nOffset;
    qint64 g_nSize;
    bool g_bSync;
};

#endif  // DIALOGDATAINSPECTOR_H
