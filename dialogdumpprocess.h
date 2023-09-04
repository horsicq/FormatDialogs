/* Copyright (c) 2019-2023 hors<horsicq@gmail.com>
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
#ifndef DIALOGDUMPPROCESS_H
#define DIALOGDUMPPROCESS_H

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QThread>

#include "dumpprocess.h"
#include "xdialogprocess.h"
#include "xoptions.h"

class DialogDumpProcess : public XDialogProcess {
    Q_OBJECT

public:
    explicit DialogDumpProcess(QWidget *pParent);
    DialogDumpProcess(QWidget *pParent, QIODevice *pDevice, qint64 nOffset, qint64 nSize, const QString &sFileName, DumpProcess::DT dumpType);
    ~DialogDumpProcess();

    void setData(QIODevice *pDevice, qint64 nOffset, qint64 nSize, const QString &sFileName, DumpProcess::DT dumpType);
    void setData(QIODevice *pDevice, const QList<DumpProcess::RECORD> &listRecords, DumpProcess::DT dumpType);
    void setData(QIODevice *pDevice, DumpProcess::RECORD record, DumpProcess::DT dumpType);

signals:
    void errorMessage(const QString &sText);  // TODO mb remove

private:
    DumpProcess *g_pDump;
    QThread *g_pThread;
};

#endif  // DIALOGDUMPPROCESS_H
