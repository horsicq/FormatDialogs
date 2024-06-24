/* Copyright (c) 2019-2024 hors<horsicq@gmail.com>
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

DialogTextInfo::DialogTextInfo(QWidget *pParent) : XShortcutsDialog(pParent), ui(new Ui::DialogTextInfo)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window);
}

DialogTextInfo::~DialogTextInfo()
{
    delete ui;
}

void DialogTextInfo::setWrap(bool bState)
{
    if (bState) {
        ui->textEditInfo->setLineWrapMode(QTextEdit::WidgetWidth);
    } else {
        ui->textEditInfo->setLineWrapMode(QTextEdit::NoWrap);
    }
}

void DialogTextInfo::setTitle(const QString &sTitle)
{
    setWindowTitle(sTitle);
}

void DialogTextInfo::setText(const QString &sText)
{
    ui->textEditInfo->setPlainText(sText);
}

void DialogTextInfo::setByteArray(const QByteArray &baData)
{
    QString sString = QString::fromUtf8(baData.data());

    if (Qt::mightBeRichText(sString)) {
        ui->textEditInfo->setHtml(sString);
    } else {
        ui->textEditInfo->setPlainText(sString);
    }
}

void DialogTextInfo::setFileName(const QString &sFileName)
{
    QFile file;
    file.setFileName(sFileName);

    if (file.open(QFile::ReadOnly)) {
        QByteArray baData = file.readAll();
        setByteArray(baData);
        file.close();
    }
}

void DialogTextInfo::setStringList(const QList<QString> listString)
{
    qint32 nNumberOfStrings = listString.count();

    QString sText;

    for (qint32 i = 0; i < nNumberOfStrings; i++) {
        sText += listString.at(i);

        if (i != (nNumberOfStrings - 1)) {
            sText += "\r\n";
        }
    }

    setText(sText);
}

void DialogTextInfo::setDevice(QIODevice *pDevice)
{
    Q_UNUSED(pDevice)
    // TODO
}

void DialogTextInfo::adjustView()
{
    // TODO
}
#ifdef USE_ARCHIVE
void DialogTextInfo::setArchive(const QString &sFileName, const QString &sRecordFileName)
{
    QByteArray baData = XArchives::decompress(sFileName, sRecordFileName);

    setByteArray(baData);
}
#endif
#ifdef USE_ARCHIVE
void DialogTextInfo::setArchive(QIODevice *pDevice, const QString &sRecordFileName)
{
    QByteArray baData = XArchives::decompress(pDevice, sRecordFileName);

    setByteArray(baData);
}
#endif
void DialogTextInfo::on_pushButtonClose_clicked()
{
    this->close();
}

void DialogTextInfo::on_pushButtonSave_clicked()
{
    QString sFilter = QString("%1 (*.txt)").arg(tr("Text documents"));
    QString sFileName = QFileDialog::getSaveFileName(this, tr("Save result"), QString("%1.txt").arg(tr("Result")), sFilter);

    if (!sFileName.isEmpty()) {
        QFile file;
        file.setFileName(sFileName);

        if (file.open(QIODevice::ReadWrite)) {
            QString sText = ui->textEditInfo->toPlainText();
            file.write(sText.toUtf8().data());
            file.close();
        }
    }
}
