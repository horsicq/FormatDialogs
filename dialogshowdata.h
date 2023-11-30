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
#ifndef DIALOGSHOWDATA_H
#define DIALOGSHOWDATA_H

#include <QDialog>
#include "xbinary.h"
#include "xoptions.h"
#include "xshortcuts.h"

namespace Ui {
class DialogShowData;
}

class DialogShowData : public QDialog {
    Q_OBJECT

    enum DTYPE {
        DTYPE_C = 0,
        DTYPE_CPP,
        DTYPE_JAVA,
        DTYPE_VBNET,
        DTYPE_BASE64,
        // TODO more
        // TODO ASM
        // TODO PASCAL
    };

public:
    explicit DialogShowData(QWidget *pParent, QIODevice *pDevice, qint64 nOffset, qint64 nSize);
    ~DialogShowData();

private slots:
    void on_pushButtonOK_clicked();
    void reload();
    void on_pushButtonCopy_clicked();
    void on_listWidgetType_currentRowChanged(int nCurrentRow);
    QString getDataString(DTYPE dtype);
    void _addItem(const QString &sName, DTYPE dtype);

private:
    Ui::DialogShowData *ui;
    QIODevice *g_pDevice;
    qint64 g_nOffset;
    qint64 g_nSize;
};

#endif  // DIALOGSHOWDATA_H
