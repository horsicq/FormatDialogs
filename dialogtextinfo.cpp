/* Copyright (c) 2019-2022 hors<horsicq@gmail.com>
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
#include "dialogtextinfo.h"
#include "ui_dialogtextinfo.h"

DialogTextInfo::DialogTextInfo(QWidget *pParent) :
    QDialog(pParent),
    ui(new Ui::DialogTextInfo)
{
    ui->setupUi(this);
}

DialogTextInfo::~DialogTextInfo()
{
    delete ui;
}

void DialogTextInfo::setWrap(bool bState)
{
    if(bState)
    {
        ui->textEditInfo->setLineWrapMode(QTextEdit::WidgetWidth);
    }
    else
    {
        ui->textEditInfo->setLineWrapMode(QTextEdit::NoWrap);
    }
}

void DialogTextInfo::setTitle(QString sTitle)
{
    setWindowTitle(sTitle);
}

void DialogTextInfo::setText(QString sText)
{
    ui->textEditInfo->setPlainText(sText);
}

void DialogTextInfo::setByteArray(QByteArray baData)
{
    QString sString=QString::fromUtf8(baData.data());

    if(Qt::mightBeRichText(sString))
    {
        ui->textEditInfo->setHtml(sString);
    }
    else
    {
        ui->textEditInfo->setPlainText(sString);
    }
}

void DialogTextInfo::setFile(QString sFileName)
{
    QFile file;

    file.setFileName(sFileName);

    if(file.open(QFile::ReadOnly))
    {
        QByteArray baData=file.readAll();
        setByteArray(baData);

        file.close();
    }
}

void DialogTextInfo::setDevice(QIODevice *pDevice)
{
    Q_UNUSED(pDevice)
    // TODO
}
#ifdef USE_ARCHIVE
void DialogTextInfo::setArchive(QString sFileName,QString sRecordFileName)
{
    QByteArray baData=XArchives::decompress(sFileName,sRecordFileName);

    setByteArray(baData);
}
#endif
void DialogTextInfo::on_pushButtonClose_clicked()
{
    this->close();
}

void DialogTextInfo::on_pushButtonSave_clicked()
{
    QString sFilter;
    sFilter+=QString("%1 (*.txt)").arg(tr("Text documents"));
    QString sFileName=QFileDialog::getSaveFileName(this,tr("Save result"),QString("%1.txt").arg(tr("Result")),sFilter);

    if(!sFileName.isEmpty())
    {
        QFile file;
        file.setFileName(sFileName);

        if(file.open(QIODevice::ReadWrite))
        {
            QString sText=ui->textEditInfo->toPlainText();

            file.write(sText.toUtf8().data());

            file.close();
        }
    }
}
