/* Copyright (c) 2022 hors<horsicq@gmail.com>
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
#ifndef XDIALOGPROCESS_H
#define XDIALOGPROCESS_H

#include <QDialog>
#include <QMessageBox>
#include <QThread>
#include <QTimer>
#include "xbinary.h"
#include "xoptions.h"

namespace Ui {
class XDialogProcess;
}

class XDialogProcess : public QDialog
{
    Q_OBJECT

public:
    explicit XDialogProcess(QWidget *pParent);
    ~XDialogProcess();

    XBinary::PDSTRUCT *getPdStruct();

    void stop();
    bool isSuccess();
    void waitForFinished();
    qint32 showDialogDelay(quint64 nMsec);

public slots:
    void errorMessage(QString sErrorMessage);
    void onCompleted(qint64 nElapsed);
    void timerSlot();

private slots:
    void on_pushButtonCancel_clicked();

private:
    Ui::XDialogProcess *ui;
    XBinary::PDSTRUCT g_pdStruct;
    QTimer *g_pTimer;
    QElapsedTimer *g_pScanTimer;
};

#endif // XDIALOGPROCESS_H
