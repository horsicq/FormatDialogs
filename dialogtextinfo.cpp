/* Copyright (c) 2019-2026 hors<horsicq@gmail.com>
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

#include <QMessageBox>
#include <QPushButton>
#include <QSaveFile>
#include <QSignalBlocker>

namespace {
constexpr qint64 MAXIMUM_CONTENT_SIZE = 64 * 1024 * 1024;
constexpr qint64 READ_CHUNK_SIZE = 64 * 1024;
}

DialogTextInfo::DialogTextInfo(QWidget *pParent) : XShortcutsDialog(pParent, true), ui(new Ui::DialogTextInfo)
{
    ui->setupUi(this);

    ui->textEditInfo->setAccessibleName(tr("Information"));
    ui->textEditInfo->setAccessibleDescription(tr("Read-only information content"));
    ui->checkBoxWrap->setAccessibleName(tr("Wrap long lines"));
    ui->labelStatus->setAccessibleName(tr("Content status"));

    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    QPushButton *pSaveButton = ui->buttonBox->button(QDialogButtonBox::Save);
    QPushButton *pCloseButton = ui->buttonBox->button(QDialogButtonBox::Close);
    pSaveButton->setDefault(false);
    pSaveButton->setAutoDefault(false);
    pCloseButton->setDefault(true);
    connect(pSaveButton, &QPushButton::clicked, this, &DialogTextInfo::saveResult);
    connect(ui->textEditInfo, &QTextEdit::textChanged, this, &DialogTextInfo::onTextChanged);

    setWrap(true);
    clearContent(tr("No information to display."));
}

DialogTextInfo::~DialogTextInfo()
{
    delete ui;
}

void DialogTextInfo::setWrap(bool bState)
{
    const QSignalBlocker blocker(ui->checkBoxWrap);
    ui->checkBoxWrap->setChecked(bState);

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

void DialogTextInfo::setHtml(const QString &sHtml)
{
    ui->textEditInfo->setHtml(sHtml);
}

void DialogTextInfo::setByteArray(const QByteArray &baData)
{
    setText(QString::fromUtf8(baData.constData(), baData.size()));
}

void DialogTextInfo::setHtmlByteArray(const QByteArray &baData)
{
    setHtml(QString::fromUtf8(baData.constData(), baData.size()));
}

void DialogTextInfo::setFileName(const QString &sFileName)
{
    loadFile(sFileName);
}

bool DialogTextInfo::loadFile(const QString &sFileName, bool bHtml)
{
    QFile file(sFileName);

    if (file.open(QFile::ReadOnly)) {
        const bool bResult = loadDevice(&file, bHtml);
        file.close();
        return bResult;
    }

    clearContent(tr("Cannot read the selected file."));
    return false;
}

void DialogTextInfo::setStringList(const QList<QString> &listString)
{
    QStringList listText;
    listText.reserve(listString.size());

    for (const QString &sText : listString) {
        listText.append(sText);
    }

    setText(listText.join(QLatin1Char('\n')));
}

void DialogTextInfo::setDevice(QIODevice *pDevice)
{
    loadDevice(pDevice);
}

bool DialogTextInfo::loadDevice(QIODevice *pDevice, bool bHtml)
{
    if (!pDevice || !pDevice->isOpen() || !pDevice->isReadable() || pDevice->isSequential()) {
        clearContent(tr("A readable random-access device is required."));
        return false;
    }

    const qint64 nOriginalPosition = pDevice->pos();
    const qint64 nSize = pDevice->size();

    if ((nOriginalPosition < 0) || (nSize < 0)) {
        clearContent(tr("The device size or position is unavailable."));
        return false;
    }

    if (nSize > MAXIMUM_CONTENT_SIZE) {
        clearContent(tr("The information is too large to display."));
        return false;
    }

    if (!pDevice->seek(0)) {
        clearContent(tr("The device cannot be read from the beginning."));
        return false;
    }

    QByteArray baData;
    baData.reserve(static_cast<int>(nSize));
    char buffer[READ_CHUNK_SIZE];
    bool bReadSuccess = true;

    while (baData.size() < nSize) {
        const qint64 nRemaining = nSize - baData.size();
        const qint64 nRead = pDevice->read(buffer, qMin(nRemaining, READ_CHUNK_SIZE));

        if (nRead <= 0) {
            bReadSuccess = false;
            break;
        }

        baData.append(buffer, static_cast<int>(nRead));
    }

    if (!pDevice->seek(nOriginalPosition)) {
        clearContent(tr("The device position could not be restored."));
        return false;
    }

    if (!bReadSuccess) {
        clearContent(tr("Cannot read the complete information from the device."));
        return false;
    }

    if (bHtml) {
        setHtmlByteArray(baData);
    } else {
        setByteArray(baData);
    }

    return true;
}

bool DialogTextInfo::saveToFile(const QString &sFileName) const
{
    QSaveFile file(sFileName);
    const QByteArray baText = ui->textEditInfo->toPlainText().toUtf8();

    return file.open(QIODevice::WriteOnly) &&
           (file.write(baText) == baText.size()) && file.commit();
}

void DialogTextInfo::adjustView()
{
    getGlobalOptions()->adjustWidget(ui->textEditInfo, XOptions::ID_VIEW_FONT_TEXTEDITS);
}
#ifdef USE_ARCHIVE
void DialogTextInfo::setArchive(const QString &sFileName, const QString &sRecordFileName)
{
    const QByteArray baData = XArchives::decompress(sFileName, sRecordFileName);

    if (baData.isEmpty()) {
        clearContent(tr("The archive record is empty or cannot be read."));
    } else {
        setByteArray(baData);
    }
}
#endif
#ifdef USE_ARCHIVE
void DialogTextInfo::setArchive(QIODevice *pDevice, const QString &sRecordFileName)
{
    if (!pDevice || !pDevice->isOpen() || !pDevice->isReadable()) {
        clearContent(tr("A readable archive device is required."));
        return;
    }

    const QByteArray baData = XArchives::decompress(pDevice, sRecordFileName);

    if (baData.isEmpty()) {
        clearContent(tr("The archive record is empty or cannot be read."));
    } else {
        setByteArray(baData);
    }
}
#endif
void DialogTextInfo::on_checkBoxWrap_toggled(bool bChecked)
{
    setWrap(bChecked);
}

void DialogTextInfo::onTextChanged()
{
    updateStatus();
}

void DialogTextInfo::saveResult()
{
    QString sFilter = QString("%1 (*.txt)").arg(tr("Text documents"));
    QString sFileName = QFileDialog::getSaveFileName(this, tr("Save result"), QString("%1.txt").arg(tr("Result")), sFilter);

    if (!sFileName.isEmpty()) {
        if (!saveToFile(sFileName)) {
            QMessageBox::critical(XOptions::getMainWidget(this), tr("Error"), QString("%1: %2").arg(tr("Cannot save file"), sFileName));
        }
    }
}

void DialogTextInfo::clearContent(const QString &sStatus)
{
    ui->textEditInfo->clear();
    ui->labelStatus->setText(sStatus);
    ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);
}

void DialogTextInfo::updateStatus()
{
    const QString sText = ui->textEditInfo->toPlainText();
    const qsizetype nCharacters = sText.size();
    const qsizetype nLines = sText.isEmpty() ? 0 : (sText.count(QLatin1Char('\n')) + 1);

    if (sText.isEmpty()) {
        ui->labelStatus->setText(tr("No information to display."));
    } else {
        ui->labelStatus->setText(tr("%1 characters, %2 lines.").arg(nCharacters).arg(nLines));
    }

    ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(!sText.isEmpty());
}

void DialogTextInfo::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
}
