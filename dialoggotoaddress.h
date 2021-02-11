// copyright (c) 2019-2021 hors<horsicq@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#ifndef DIALOGGOTOADDRESS_H
#define DIALOGGOTOADDRESS_H

#include <QDialog>
#include "xbinary.h"

namespace Ui
{
class DialogGoToAddress;
}

class DialogGoToAddress : public QDialog
{
    Q_OBJECT

public:

    enum TYPE
    {
        TYPE_ADDRESS=0,
        TYPE_RELADDRESS,
        TYPE_OFFSET
    };

    DialogGoToAddress(QWidget *pParent,XBinary::_MEMORY_MAP *pMemoryMap,TYPE type);
    DialogGoToAddress(QWidget *pParent,qint64 nMinValue,qint64 nMaxValue,TYPE type);
    ~DialogGoToAddress();
    qint64 getValue();

private:
    void adjustTitle(TYPE type);

private slots:
    void on_pushButtonCancel_clicked();
    void on_pushButtonOK_clicked();

private:
    Ui::DialogGoToAddress *ui;
    TYPE g_type;
    XBinary::_MEMORY_MAP *g_pMemoryMap;
    qint64 g_nMinValue;
    qint64 g_nMaxValue;
    qint64 g_nValue;
};

#endif // DIALOGGOTOADDRESS_H
