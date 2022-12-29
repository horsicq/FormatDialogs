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
#include "xdialogprocess.h"

#include "ui_xdialogprocess.h"

XDialogProcess::XDialogProcess(QWidget *pParent) : QDialog(pParent), ui(new Ui::XDialogProcess)
{
    ui->setupUi(this);

    g_pdStruct = XBinary::createPdStruct();

    ui->progressBar0->hide();
    ui->progressBar1->hide();
    ui->progressBar2->hide();
    ui->progressBar3->hide();
    ui->progressBar4->hide();

    g_pTimer = new QTimer(this);
    connect(g_pTimer, SIGNAL(timeout()), this, SLOT(timerSlot()));

    g_pTimer->start(1000);  // TODO const !!!

    g_pScanTimer = new QElapsedTimer;
    g_pScanTimer->start();
}

XDialogProcess::~XDialogProcess()
{
    delete g_pScanTimer;

    stop();

    delete ui;
}

XBinary::PDSTRUCT *XDialogProcess::getPdStruct()
{
    return &g_pdStruct;
}

void XDialogProcess::stop()
{
    g_pTimer->stop();
    g_pdStruct.bIsStop = true;
}

bool XDialogProcess::isSuccess()
{
    bool bResult = false;

    bResult = XBinary::isPdStructSuccess(getPdStruct());

    return bResult;
}

void XDialogProcess::waitForFinished()
{
    while (true) {
        QThread::msleep(50);  // TODO Consts

        bool bResult = false;

        bResult = XBinary::isPdStructFinished(getPdStruct());

        if (bResult) {
            break;
        }
    }
}

void XDialogProcess::errorMessage(QString sText)
{
    QMessageBox::critical(XOptions::getMainWidget(this), tr("Error"), sText);
}

void XDialogProcess::onCompleted(qint64 nElapsed)
{
    Q_UNUSED(nElapsed)

    if (isSuccess()) {
        accept();
    } else {
        reject();
    }
}

void XDialogProcess::timerSlot()
{
    setupProgressBar(0, ui->progressBar0);
    setupProgressBar(1, ui->progressBar1);
    setupProgressBar(2, ui->progressBar2);
    setupProgressBar(3, ui->progressBar3);
    setupProgressBar(4, ui->progressBar4);

    QTime _time = QTime(0, 0);
    _time = _time.addMSecs(g_pScanTimer->elapsed());
    QString sTime = _time.toString();
    // TODO
    // If more 60 sec add min
    // If more 60 min add hours

    ui->labelTime->setText(sTime);
}

void XDialogProcess::setupProgressBar(qint32 nIndex, QProgressBar *pProgressBar)
{
    if (getPdStruct()->_pdRecord[nIndex].bIsValid) {
        pProgressBar->show();

        QString sStatus;

        pProgressBar->setMaximum(100);

        if (getPdStruct()->_pdRecord[nIndex].nTotal) {
            pProgressBar->setValue((getPdStruct()->_pdRecord[nIndex].nCurrent * 100) / (getPdStruct()->_pdRecord[nIndex].nTotal));

            sStatus += QString("[%1/%2] ").arg(QString::number(getPdStruct()->_pdRecord[nIndex].nCurrent), QString::number(getPdStruct()->_pdRecord[nIndex].nTotal));
        } else {
            pProgressBar->setValue(0);
        }

        sStatus += getPdStruct()->_pdRecord[nIndex].sStatus;

        pProgressBar->setFormat(sStatus);
    } else {
        pProgressBar->hide();
    }
}

qint32 XDialogProcess::showDialogDelay(quint64 nMsec)
{
    // mb TODO options
    qint32 nResult = Accepted;

    for (quint64 i = 0; i < nMsec; i += 50) {
        if (i) {
            QThread::msleep(50);
        } else {
            QThread::msleep(10);
        }

        if (isSuccess()) {
            break;
        }
    }

    if (!isSuccess()) {
        nResult = exec();
    }

    return nResult;
}

void XDialogProcess::on_pushButtonCancel_clicked()
{
    stop();
}
