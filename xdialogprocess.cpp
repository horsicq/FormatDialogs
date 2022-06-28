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

XDialogProcess::XDialogProcess(QWidget *pParent) :
    QDialog(pParent),
    ui(new Ui::XDialogProcess)
{
    ui->setupUi(this);

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(0);
    ui->progressBarOpt->setMinimum(0);
    ui->progressBarOpt->setMaximum(0);

    g_pdStruct={};

    g_pTimer=new QTimer(this);
    connect(g_pTimer,SIGNAL(timeout()),this,SLOT(timerSlot()));

    g_pTimer->start(1000); // TODO constt
}

XDialogProcess::~XDialogProcess()
{
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
    g_pdStruct.bIsStop=true;
}

bool XDialogProcess::isSuccess()
{
    bool bResult=false;

    if(getPdStruct()->pdRecordOpt.bIsValid)
    {
        bResult=g_pdStruct.pdRecordOpt.bSuccess;
    }
    else
    {
        bResult=g_pdStruct.pdRecord.bSuccess;
    }

    return bResult;
}

void XDialogProcess::errorMessage(QString sText)
{
    QMessageBox::critical(XOptions::getMainWidget(this),tr("Error"),sText);
}

void XDialogProcess::onCompleted(qint64 nElapsed)
{
    Q_UNUSED(nElapsed)

    if(isSuccess())
    {
        accept();
    }
    else
    {
        reject();
    }
}

void XDialogProcess::timerSlot()
{
    if(getPdStruct()->pdRecord.nTotal)
    {
        ui->progressBar->setMaximum(1000);
        ui->progressBar->setValue((getPdStruct()->pdRecord.nCurrent*1000)/(getPdStruct()->pdRecord.nTotal));
    }

    if(getPdStruct()->pdRecordOpt.nTotal)
    {
        ui->progressBarOpt->setMaximum(1000);
        ui->progressBarOpt->setValue((getPdStruct()->pdRecordOpt.nCurrent*1000)/(getPdStruct()->pdRecordOpt.nTotal));
    }

    ui->labelStatus1->setText(getPdStruct()->pdRecord.sStatus);
    ui->labelStatus2->setText(getPdStruct()->pdRecordOpt.sStatus);
    ui->labelStatus3->setText(getPdStruct()->sStatus);
    ui->labelTotal->setText(QString::number(getPdStruct()->pdRecord.nTotal));
    ui->labelCurrent->setText(QString::number(getPdStruct()->pdRecord.nCurrent));
    ui->labelTotalOpt->setText(QString::number(getPdStruct()->pdRecordOpt.nTotal));
    ui->labelCurrentOpt->setText(QString::number(getPdStruct()->pdRecordOpt.nCurrent));

    // TODO time
}

qint32 XDialogProcess::showDialogDelay(quint64 nMsec)
{
    qint32 nResult=Accepted;

    for(quint64 i=0;i<nMsec;i+=50)
    {
        if(i)
        {
            QThread::msleep(50);
        }

        if(isSuccess())
        {
            break;
        }
    }

    if(!isSuccess())
    {
        nResult=exec();
    }

    return nResult;
}

void XDialogProcess::on_pushButtonCancel_clicked()
{
    stop();
}

