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
#ifndef DIALOGSEARCH_H
#define DIALOGSEARCH_H

#include "xshortcutsdialog.h"
#include "searchprocess.h"

namespace Ui {
class DialogSearch;
}

class DialogSearch : public XShortcutsDialog {
    Q_OBJECT

public:
    struct OPTIONS {
        bool bShowBegin = false;
    };

    explicit DialogSearch(QWidget *pParent, QIODevice *pDevice, XBinary::SEARCHDATA *pSearchData, XBinary::SEARCHMODE searchMode, const OPTIONS &options);
    ~DialogSearch() override;

    void adjustView() override;

private slots:
    void acceptSearch();
    void updateModeState();
    void updateValue();
    void checkValid();

protected:
    void registerShortcuts(bool bState) override;

private:
    XBinary::VT currentValueType() const;
    bool readValue(QVariant *pValue, QString *pHex, QString *pError) const;
    void setStatus(const QString &sText);

    Ui::DialogSearch *ui;
    XBinary::SEARCHDATA *m_pSearchData;
};

#endif  // DIALOGSEARCH_H
