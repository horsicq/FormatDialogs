/* Copyright (c) 2022-2026 hors<horsicq@gmail.com>
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

    m_pThreadObject = nullptr;
    m_pThread = nullptr;
    m_bSuccess = false;

    memset(m_nSpeed, 0, sizeof m_nSpeed);

    m_pdStruct = XBinary::createPdStruct();

    ui->progressBar0->hide();
    ui->progressBar1->hide();
    ui->progressBar2->hide();
    ui->progressBar3->hide();
    ui->progressBar4->hide();

    ui->labelRemain->hide();

    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(timerSlot()));

    m_pTimer->start(N_TIMER_MS);

    m_pScanTimer = new QElapsedTimer;
    m_pScanTimer->start();

    setAdvanced(true);
}

XDialogProcess::XDialogProcess(QWidget *pParent, XThreadObject *pThreadObject) : XDialogProcess(pParent)
{
    m_pThreadObject = pThreadObject;
    m_pThread = new QThread;

    m_pThreadObject->moveToThread(m_pThread);

    connect(m_pThread, SIGNAL(started()), m_pThreadObject, SLOT(_process()));
    connect(m_pThreadObject, SIGNAL(completed(qint64)), this, SLOT(onCompleted(qint64)));
    connect(m_pThreadObject, SIGNAL(errorMessage(QString)), this, SLOT(errorMessageSlot(QString)));
    // connect(m_pThreadObject, SIGNAL(warningMessage(QString)), this, SLOT(warningMessageSlot(QString)));
    // connect(m_pThreadObject, SIGNAL(infoMessage(QString)), this, SLOT(infoMessageSlot(QString)));

    setWindowTitle(m_pThreadObject->getTitle());
}

XDialogProcess::~XDialogProcess()
{
    delete m_pScanTimer;

    stop();

    if (m_pThread) {
        m_pThread->quit();
        m_pThread->wait();
        delete m_pThread;
    }

    delete ui;
}

XBinary::PDSTRUCT *XDialogProcess::getPdStruct()
{
    return &m_pdStruct;
}

void XDialogProcess::stop()
{
    m_pdStruct.bIsStop = true;
}

bool XDialogProcess::isSuccess()
{
    return m_bSuccess;
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

    m_pTimer->stop();

    if (!XBinary::isPdStructStopped(&m_pdStruct)) {
        m_bSuccess = true;
        accept();
    } else {
        m_bSuccess = false;
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

    qint64 nElapsed = m_pScanTimer->elapsed();

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
            m_nSpeed[nIndex] = 0;
        }

        m_nSpeed[nIndex]++;

        if (m_nSpeed[nIndex]) {
            quint64 nCurrent = getPdStruct()->_pdRecord[nIndex].nCurrent;

            double dCurrent = (double)nCurrent / m_nSpeed[nIndex];
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

    if (m_pdStruct.sInfoString != "") {
        QMessageBox::information(XOptions::getMainWidget(this), tr("Info"), m_pdStruct.sInfoString);
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
    if (m_pThread) {
        m_pThread->start();
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

XThreadObject *XDialogProcess::getThreadObject()
{
    return m_pThreadObject;
}
