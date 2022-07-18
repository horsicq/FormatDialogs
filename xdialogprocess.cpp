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
    ui->progressBarObj->setMinimum(0);
    ui->progressBarObj->setMaximum(0);
    ui->progressBarFiles->setMinimum(0);
    ui->progressBarFiles->setMaximum(0);

    ui->groupBoxOpt->hide();
    ui->groupBoxObj->hide();
    ui->groupBoxFiles->hide();

    g_pdStruct={};

    g_pTimer=new QTimer(this);
    connect(g_pTimer,SIGNAL(timeout()),this,SLOT(timerSlot()));

    g_pTimer->start(1000); // TODO const

    g_pScanTimer=new QElapsedTimer;
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
    g_pdStruct.bIsStop=true;
}

bool XDialogProcess::isSuccess()
{
    bool bResult=false;

    if(getPdStruct()->pdRecordFiles.bIsValid)
    {
        bResult=g_pdStruct.pdRecordFiles.bSuccess;
    }
    else if(getPdStruct()->pdRecordObj.bIsValid)
    {
        bResult=g_pdStruct.pdRecordObj.bSuccess;
    }
    else if(getPdStruct()->pdRecordOpt.bIsValid)
    {
        bResult=g_pdStruct.pdRecordOpt.bSuccess;
    }
    else
    {
        bResult=g_pdStruct.pdRecord.bSuccess;
    }

    return bResult;
}

void XDialogProcess::waitForFinished()
{
    while(true)
    {
        QThread::msleep(50);

        bool bResult=false;

        if(getPdStruct()->pdRecordFiles.bIsValid)
        {
            bResult=g_pdStruct.pdRecordFiles.bFinished;
        }
        else if(getPdStruct()->pdRecordObj.bIsValid)
        {
            bResult=g_pdStruct.pdRecordObj.bFinished;
        }
        else if(getPdStruct()->pdRecordOpt.bIsValid)
        {
            bResult=g_pdStruct.pdRecordOpt.bFinished;
        }
        else
        {
            bResult=g_pdStruct.pdRecord.bFinished;
        }

        if(bResult)
        {
            break;
        }
    }
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
    ui->labelStatus->setText(getPdStruct()->sStatus);

    if(getPdStruct()->pdRecord.nTotal)
    {
        ui->progressBar->setMaximum(100);
        ui->progressBar->setValue((getPdStruct()->pdRecord.nCurrent*100)/(getPdStruct()->pdRecord.nTotal));
    }

    ui->groupBox->setTitle(QString("[%1/%2] %3").arg(QString::number(getPdStruct()->pdRecord.nTotal),QString::number(getPdStruct()->pdRecord.nCurrent),getPdStruct()->pdRecord.sStatus));

    if(getPdStruct()->pdRecordOpt.bIsValid)
    {
        ui->groupBoxOpt->show();

        ui->groupBoxOpt->setTitle(QString("[%1/%2] %3").arg(QString::number(getPdStruct()->pdRecordOpt.nTotal),QString::number(getPdStruct()->pdRecordOpt.nCurrent),getPdStruct()->pdRecordOpt.sStatus));

        if(getPdStruct()->pdRecordOpt.nTotal)
        {
            ui->progressBarOpt->setMaximum(100);
            ui->progressBarOpt->setValue((getPdStruct()->pdRecordOpt.nCurrent*100)/(getPdStruct()->pdRecordOpt.nTotal));
        }
    }
    else
    {
        ui->groupBoxOpt->hide();
    }

    if(getPdStruct()->pdRecordObj.bIsValid)
    {
        ui->groupBoxObj->show();

        ui->groupBoxObj->setTitle(QString("[%1/%2] %3").arg(QString::number(getPdStruct()->pdRecordObj.nTotal),QString::number(getPdStruct()->pdRecordObj.nCurrent),getPdStruct()->pdRecordObj.sStatus));

        if(getPdStruct()->pdRecordObj.nTotal)
        {
            ui->progressBarObj->setMaximum(100);
            ui->progressBarObj->setValue((getPdStruct()->pdRecordObj.nCurrent*100)/(getPdStruct()->pdRecordObj.nTotal));
        }
    }
    else
    {
        ui->groupBoxObj->hide();
    }

    if(getPdStruct()->pdRecordFiles.bIsValid)
    {
        ui->groupBoxFiles->show();

        ui->groupBoxFiles->setTitle(QString("[%1/%2] %3").arg(QString::number(getPdStruct()->pdRecordFiles.nTotal),QString::number(getPdStruct()->pdRecordFiles.nCurrent),getPdStruct()->pdRecordFiles.sStatus));

        if(getPdStruct()->pdRecordFiles.nTotal)
        {
            ui->progressBarFiles->setMaximum(100);
            ui->progressBarFiles->setValue((getPdStruct()->pdRecordFiles.nCurrent*100)/(getPdStruct()->pdRecordFiles.nTotal));
        }
    }
    else
    {
        ui->groupBoxFiles->hide();
    }

    QTime _time=QTime(0,0);
    _time=_time.addMSecs(g_pScanTimer->elapsed());
    QString sTime=_time.toString();
    // TODO if more 60 sec add min
    // If more 60 min add hours

    ui->labelTime->setText(sTime);
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

