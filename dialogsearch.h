/* Copyright (c) 2019-2022 hors<horsicq@gmail.com>
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
#ifndef DIALOGSEARCH_H
#define DIALOGSEARCH_H

#include <QDialog>

#include "dialogsearchprocess.h"

namespace Ui {
class DialogSearch;
}

class DialogSearch : public QDialog {
    Q_OBJECT

public:
    enum SEARCHMODE {
        SEARCHMODE_STRING = 0,
        SEARCHMODE_SIGNATURE,
        SEARCHMODE_VALUE
    };

    explicit DialogSearch(QWidget *pParent, QIODevice *pDevice, XBinary::SEARCHDATA *pSearchData, SEARCHMODE searchMode);
    ~DialogSearch();

private slots:
    void on_pushButtonCancel_clicked();
    void on_pushButtonOK_clicked();
    void on_tabWidgetSearch_currentChanged(int nIndex);
    void on_lineEditValue_textChanged(const QString &sText);
    void on_comboBoxEndianness_currentIndexChanged(int nIndex);
    void on_radioButtonChar_toggled(bool bChecked);
    void on_radioButtonUchar_toggled(bool bChecked);
    void on_radioButtonShort_toggled(bool bChecked);
    void on_radioButtonUshort_toggled(bool bChecked);
    void on_radioButtonInt_toggled(bool bChecked);
    void on_radioButtonUint_toggled(bool bChecked);
    void on_radioButtonInt64_toggled(bool bChecked);
    void on_radioButtonUint64_toggled(bool bChecked);
    void on_radioButtonFloat_toggled(bool bChecked);
    void on_radioButtonDouble_toggled(bool bChecked);
    void ajustValue();
    void checkValid();
    void on_plainTextEditString_textChanged();
    void on_plainTextEditSignature_textChanged();

private:
    Ui::DialogSearch *ui;
    QIODevice *g_pDevice;
    XBinary::SEARCHDATA *g_pSearchData;
};

#endif  // DIALOGSEARCH_H
