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
#ifndef DIALOGHEXSIGNATURE_H
#define DIALOGHEXSIGNATURE_H

#include <QByteArray>

#include "dialogsearchsignatures.h"
#include "dialogsearchvalues.h"
#include "xshortcutsdialog.h"

class QEvent;
class QIODevice;
class QTableWidgetItem;

namespace Ui {
class DialogHexSignature;
}

class DialogHexSignature : public XShortcutsDialog {
    Q_OBJECT

    static const qint32 G_N_MAX_BYTES = 128;

public:
    explicit DialogHexSignature(QWidget *pParent, QIODevice *pDevice, qint64 nOffset, qint64 nSize);
    ~DialogHexSignature() override;

    void adjustView() override;

private slots:
    void reload();
    void on_pushButtonCopy_clicked();
    void on_checkBoxSpaces_toggled(bool bChecked);
    void on_checkBoxUpper_toggled(bool bChecked);
    void on_lineEditWildcard_textChanged(const QString &sText);
    void on_pushButtonScan_clicked();
    void on_checkBoxANSI_toggled(bool bChecked);
    void on_pushButtonClearWildcards_clicked();
    void on_tableWidgetBytes_cellClicked(qint32 nRow, qint32 nColumn);

protected:
    void registerShortcuts(bool bState) override;
    bool eventFilter(QObject *pWatched, QEvent *pEvent) override;

private:
    enum ITEM_ROLE {
        ITEM_ROLE_BYTE_VALUE = Qt::UserRole,
        ITEM_ROLE_WILDCARD
    };

    QString buildSignature() const;
    void setWildcardState(QTableWidgetItem *pItem, bool bState);
    void updateByteItem(QTableWidgetItem *pItem);
    qint32 getWildcardCount() const;
    bool isSignatureValid(const QString &sSignature) const;

    Ui::DialogHexSignature *ui;
    QByteArray m_baData;
    QIODevice *m_pDevice;
    qint64 m_nOffset;
};

#endif  // DIALOGHEXSIGNATURE_H
