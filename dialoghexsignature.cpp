// copyright (c) 2019-2021 hors<horsicq@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include "dialoghexsignature.h"
#include "ui_dialoghexsignature.h"

DialogHexSignature::DialogHexSignature(QWidget *pParent,QIODevice *pDevice,qint64 nOffset,qint64 nSize,QString sSignaturesPath) :
    XShortcutsDialog(pParent),
    ui(new Ui::DialogHexSignature)
{
    ui->setupUi(this);

    g_pDevice=pDevice;
    g_sSignaturesPath=sSignaturesPath;

    ui->textEditSignature->setWordWrapMode(QTextOption::WrapAnywhere);

    for(int i=0;i<G_N_MAX;i++)
    {
        pushButton[i]=new QPushButton;
        pushButton[i]->setMaximumWidth(30);
        pushButton[i]->setMaximumHeight(20);
        pushButton[i]->setCheckable(true);
        pushButton[i]->setEnabled(false);

        connect(pushButton[i],SIGNAL(toggled(bool)),this,SLOT(reload()));

        if((i>=0)&&(i<16)) // TODO Consts
        {
            ui->horizontalLayout0->addWidget(pushButton[i]);
        }
        else if((i>=16)&&(i<32))
        {
            ui->horizontalLayout1->addWidget(pushButton[i]);
        }
        else if((i>=32)&&(i<48))
        {
            ui->horizontalLayout2->addWidget(pushButton[i]);
        }
        else if((i>=48)&&(i<64))
        {
            ui->horizontalLayout3->addWidget(pushButton[i]);
        }
        else if((i>=64)&&(i<80))
        {
            ui->horizontalLayout4->addWidget(pushButton[i]);
        }
        else if((i>=80)&&(i<96))
        {
            ui->horizontalLayout5->addWidget(pushButton[i]);
        }
        else if((i>=96)&&(i<112))
        {
            ui->horizontalLayout6->addWidget(pushButton[i]);
        }
        else if((i>=112)&&(i<128))
        {
            ui->horizontalLayout7->addWidget(pushButton[i]);
        }
    }

    nSize=qMin(nSize,(qint64)128);

    g_baData=XBinary::read_array(pDevice,nOffset,nSize);

    for(int i=0; i<nSize; i++)
    {
        pushButton[i]->setText(QString("%1").arg((unsigned char)(g_baData.data()[i]),2,16,QChar('0')).toUpper());
        pushButton[i]->setEnabled(true);
    }

    reload();

    XOptions::setMonoFont(ui->textEditSignature);
}

DialogHexSignature::~DialogHexSignature()
{
    delete ui;
}

void DialogHexSignature::on_pushButtonOK_clicked()
{
    this->close();
}

void DialogHexSignature::reload()
{
    int _nMax=G_N_MAX;
    int nSize=qMin(_nMax,g_baData.size());
    QString sSignature;
    QString sTemp;

    for(int i=0; i<nSize; i++)
    {
        if(pushButton[i]->isChecked())
        {
            sTemp=ui->lineEditWildcard->text();
            sTemp+=sTemp;
        }
        else
        {
            sTemp=QString("%1").arg((unsigned char)(g_baData.data()[i]),2,16,QChar('0'));
        }

        if(ui->checkBoxUpper->isChecked())
        {
            sTemp=sTemp.toUpper();
        }

        sSignature+=sTemp;

        if(ui->checkBoxSpaces->isChecked())
        {
            sSignature+=" ";
        }
    }

    ui->textEditSignature->setText(sSignature);
}

void DialogHexSignature::on_pushButtonCopy_clicked()
{
    QClipboard *clipboard=QApplication::clipboard();
    clipboard->setText(ui->textEditSignature->toPlainText());
}

void DialogHexSignature::on_checkBoxSpaces_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)

    reload();
}

void DialogHexSignature::on_checkBoxUpper_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)

    reload();
}

void DialogHexSignature::on_lineEditWildcard_textChanged(const QString &sText)
{
    Q_UNUSED(sText)

    reload();
}

void DialogHexSignature::on_pushButtonScan_clicked()
{
    SearchSignaturesWidget::OPTIONS options={};
    options.bMenu_Hex=false;
    options.sSignaturesPath=g_sSignaturesPath;
    options.sUserSignature=ui->textEditSignature->toPlainText();

    DialogSearchSignatures dialogSearchSignatures(this);

    dialogSearchSignatures.setData(g_pDevice,XBinary::FT_BINARY,options,true);
    dialogSearchSignatures.setShortcuts(getShortcuts());

    dialogSearchSignatures.exec();
}
