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
#ifndef DIALOGHEXSIGNATURE_H
#define DIALOGHEXSIGNATURE_H

#include <QClipboard>
#include <QDialog>
#include <QPushButton>

#include "dialogsearchsignatures.h"
#include "dialogsearchvalues.h"
#include "xbinary.h"
#include "xoptions.h"
#include "xshortcuts.h"

namespace Ui {
class DialogHexSignature;
}

class DialogHexSignature : public XShortcutsDialog {
    Q_OBJECT

    static const qint32 G_N_MAX_BYTES = 128;

public:
    explicit DialogHexSignature(QWidget *pParent, QIODevice *pDevice, qint64 nOffset, qint64 nSize);
    ~DialogHexSignature();

    virtual void adjustView();

private slots:
    void on_pushButtonOK_clicked();
    void reload();
    void on_pushButtonCopy_clicked();
    void on_checkBoxSpaces_toggled(bool bChecked);
    void on_checkBoxUpper_toggled(bool bChecked);
    void on_lineEditWildcard_textChanged(const QString &sText);
    void on_pushButtonScan_clicked();
    void on_checkBoxANSI_toggled(bool bChecked);

protected:
    virtual void registerShortcuts(bool bState);

private:
    Ui::DialogHexSignature *ui;
    QPushButton *g_pushButton[G_N_MAX_BYTES];
    QByteArray g_baData;
    QIODevice *m_pDevice;
};

#endif  // DIALOGHEXSIGNATURE_H
