// copyright (c) 2019-2020 hors<horsicq@gmail.com>
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
#include "dialogsearch.h"
#include "ui_dialogsearch.h"

DialogSearch::DialogSearch(QWidget *parent, QIODevice *pDevice, SearchProcess::SEARCHDATA *pSearchData) :
    QDialog(parent),
    ui(new Ui::DialogSearch)
{
    ui->setupUi(this);

    this->pSearchData=pSearchData;

    ui->comboBoxSearchFrom->addItem(tr("Begin"));
    ui->comboBoxSearchFrom->addItem(tr("Cursor"));

    ui->comboBoxType->addItem(tr("ANSI"));
    ui->comboBoxType->addItem(tr("Unicode"));
}

DialogSearch::~DialogSearch()
{
    delete ui;
}

void DialogSearch::on_pushButtonCancel_clicked()
{
    this->close();
}

void DialogSearch::on_pushButtonOK_clicked()
{
    // TODO save state
    if(ui->tabWidgetSearch->currentIndex()==0) // Strings
    {
        // TODO
        bool bMatchCase=ui->checkBoxMatchCase->isChecked();

        if(ui->comboBoxType->currentIndex()==0) // ANSI
        {
            if(bMatchCase)
            {
                pSearchData->type=SearchProcess::TYPE_ANSISTRING;
            }
            else
            {
                pSearchData->type=SearchProcess::TYPE_ANSISTRING_I;
            }
        }
        else if(ui->comboBoxType->currentIndex()==1) // UNICODE
        {
            if(bMatchCase)
            {
                pSearchData->type=SearchProcess::TYPE_UNICODESTRING;
            }
            else
            {
                pSearchData->type=SearchProcess::TYPE_UNICODESTRING_I;
            }
        }
    }
    else if(ui->tabWidgetSearch->currentIndex()==1) // Signature
    {
        pSearchData->type=SearchProcess::TYPE_SIGNATURE;
        pSearchData->variant=ui->plainTextEditSignature->toPlainText();
    }
    else if(ui->tabWidgetSearch->currentIndex()==2) // Value
    {
        // TODO
    }

    // TODO

    done(QDialog::Accepted);
}
