/* Copyright (c) 2022-2025 hors<horsicq@gmail.com>
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

XDialogProcess::XDialogProcess(QWidget *pParent) : XShortcutsDialog(pParent, false), ui(new Ui::XDialogProcess)
{
    ui->setupUi(this);

    g_pThreadObject = nullptr;
    g_pThread = nullptr;

    memset(g_nSpeed, 0, sizeof g_nSpeed);

    g_pdStruct = XBinary::createPdStruct();

    ui->progressBar0->hide();
    ui->progressBar1->hide();
    ui->progressBar2->hide();
    ui->progressBar3->hide();
    ui->progressBar4->hide();

    ui->labelRemain->hide();

    g_pTimer = new QTimer(this);
    connect(g_pTimer, SIGNAL(timeout()), this, SLOT(timerSlot()));

    g_pTimer->start(N_TIMER_MS);

    g_pScanTimer = new QElapsedTimer;
    g_pScanTimer->start();

    setAdvanced(true);
}

XDialogProcess::XDialogProcess(QWidget *pParent, XThreadObject *pThreadObject) : XDialogProcess(pParent)
{
    g_pThreadObject = pThreadObject;
    g_pThread = new QThread;

    g_pThreadObject->moveToThread(g_pThread);

    connect(g_pThread, SIGNAL(started()), g_pThreadObject, SLOT(_process()));
    connect(g_pThreadObject, SIGNAL(completed(qint64)), this, SLOT(onCompleted(qint64)));

    connect(pThreadObject, SIGNAL(completed(qint64)), this, SLOT(onCompleted(qint64)));
    connect(pThreadObject, SIGNAL(errorMessage(QString)), this, SLOT(errorMessageSlot(QString)));
    // connect(pThreadObject, SIGNAL(warningMessage(QString)), this, SLOT(warningMessageSlot(QString)));
    // connect(pThreadObject, SIGNAL(infoMessage(QString)), this, SLOT(infoMessageSlot(QString)));

    setWindowTitle(pThreadObject->getTitle());
}

XDialogProcess::~XDialogProcess()
{
    delete g_pScanTimer;

    stop();

    if (g_pThread) {
        g_pThread->quit();
        g_pThread->wait();
        delete g_pThread;
    }

    delete ui;
}

XBinary::PDSTRUCT *XDialogProcess::getPdStruct()
{
    return &g_pdStruct;
}

void XDialogProcess::stop()
{
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
        QThread::msleep(50);

        bool bResult = false;

        bResult = XBinary::isPdStructFinished(getPdStruct());

        if (bResult) {
            break;
        }
    }
}

void XDialogProcess::errorMessageSlot(const QString &sText)
{
    QMessageBox::critical(XOptions::getMainWidget(this), tr("Error"), sText);
}

void XDialogProcess::onCompleted(qint64 nElapsed)
{
    Q_UNUSED(nElapsed)

    g_pTimer->stop();

    if (isSuccess()) {
        accept();
    } else {
        reject();
    }
}

void XDialogProcess::timerSlot()
{
    bool bIsEnabled = ui->checkBoxAdvanced->isChecked();

    setupProgressBar(0, ui->progressBar0, ui->labelSpeed0, true);
    setupProgressBar(1, ui->progressBar1, ui->labelSpeed1, bIsEnabled);
    setupProgressBar(2, ui->progressBar2, ui->labelSpeed2, bIsEnabled);
    setupProgressBar(3, ui->progressBar3, ui->labelSpeed3, bIsEnabled);
    setupProgressBar(4, ui->progressBar4, ui->labelSpeed4, bIsEnabled);

    qint64 nElapsed = g_pScanTimer->elapsed();

    if (nElapsed) {
        ui->labelTime->setText(XBinary::msecToDate(nElapsed));
    }

    if (getPdStruct()->_pdRecord[0].nTotal) {
        double dPercentage = (double)(getPdStruct()->_pdRecord[0].nCurrent) / (double)(getPdStruct()->_pdRecord[0].nTotal);

        if (dPercentage > 0) {
            qint64 nRemain = (qint64)(nElapsed / dPercentage) - nElapsed;

            if (nRemain > 0) {
                ui->labelRemain->show();

                ui->labelRemain->setText(XBinary::msecToDate(nRemain));
            } else {
                ui->labelRemain->hide();
            }
        } else {
            ui->labelRemain->hide();
        }
    }
}

void XDialogProcess::setupProgressBar(qint32 nIndex, QProgressBar *pProgressBar, QLabel *pLabel, bool bIsEnabled)
{
    if ((getPdStruct()->_pdRecord[nIndex].bIsValid) && (bIsEnabled)) {
        pProgressBar->show();
        pLabel->show();

        QString sStatus;

        pProgressBar->setMaximum(100);

        if (getPdStruct()->_pdRecord[nIndex].nTotal) {
            qint32 nValue = (getPdStruct()->_pdRecord[nIndex].nCurrent * 100) / (getPdStruct()->_pdRecord[nIndex].nTotal);
            pProgressBar->setValue(nValue);

            sStatus += QString("[%1/%2] ").arg(QString::number(getPdStruct()->_pdRecord[nIndex].nCurrent), QString::number(getPdStruct()->_pdRecord[nIndex].nTotal));
        } else {
            pProgressBar->setValue(0);
        }

        if (getPdStruct()->_pdRecord[nIndex].nCurrent == 0) {
            g_nSpeed[nIndex] = 0;
        }

        g_nSpeed[nIndex]++;

        if (g_nSpeed[nIndex]) {
            quint64 nCurrent = getPdStruct()->_pdRecord[nIndex].nCurrent;

            double dCurrent = (double)nCurrent / g_nSpeed[nIndex];
            pLabel->setText(QString::number(dCurrent, 'f', 2));
        }

        QString _sStatus = getPdStruct()->_pdRecord[nIndex].sStatus;

        sStatus += _sStatus;

        pProgressBar->setFormat(sStatus);
    } else {
        pProgressBar->hide();
        pLabel->hide();
    }
}

void XDialogProcess::setAdvanced(bool bState)
{
    ui->checkBoxAdvanced->setChecked(bState);
}

qint32 XDialogProcess::showDialogDelay(quint64 nMsec)
{
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

    if (g_pdStruct.sInfoString != "") {
        QMessageBox::information(XOptions::getMainWidget(this), tr("Info"), g_pdStruct.sInfoString);
    }

    if (!isSuccess()) {
        nResult = exec();
    }

    return nResult;
}

void XDialogProcess::adjustView()
{
}

void XDialogProcess::start()
{
    if (g_pThread) {
        g_pThread->start();
    }
}

void XDialogProcess::on_pushButtonCancel_clicked()
{
    stop();
}

void XDialogProcess::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
}
