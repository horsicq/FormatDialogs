// copyright (c) 2019-2020 hors<horsicq@gmail.com>
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
#ifndef DIALOGHEXSIGNATURE_H
#define DIALOGHEXSIGNATURE_H

#include <QDialog>
#include <QPushButton>
#include <QClipboard>
#include "xbinary.h"

namespace Ui {
class DialogHexSignature;
}

class DialogHexSignature : public QDialog
{
    Q_OBJECT

public:
    explicit DialogHexSignature(QWidget *parent=nullptr);
    ~DialogHexSignature();

    void setData(QIODevice *pDevice,qint64 nOffset,qint64 nSize);

private slots:
    void on_pushButtonOK_clicked();
    void reload();
    void on_pushButtonCopy_clicked();
    void on_checkBoxSpaces_toggled(bool checked);
    void on_checkBoxUpper_toggled(bool checked);
    void on_lineEditWildcard_textChanged(const QString &arg1);

private:
    Ui::DialogHexSignature *ui;
    QPushButton *pushButton[128];
    QByteArray baData;
};

#endif // DIALOGHEXSIGNATURE_H
