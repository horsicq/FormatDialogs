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
#include "dialogsearch.h"

#include "ui_dialogsearch.h"

#include <QDialogButtonBox>
#include <QFontDatabase>
#include <QLineEdit>
#include <QPushButton>
#include <cstring>
#include <limits>

namespace {
const qint32 MAX_SEARCH_TEXT_LENGTH = 256;

QString formatHexBytes(const QString &sHex)
{
    const QString sUpper = sHex.toUpper();
    QString sResult;
    sResult.reserve(sUpper.size() + (sUpper.size() / 2));

    for (qint32 i = 0; i < sUpper.size(); i++) {
        if ((i > 0) && ((i % 2) == 0)) {
            sResult.append(' ');
        }

        sResult.append(sUpper.at(i));
    }

    return sResult;
}

QString valueToByteHex(quint64 nValue, qint32 nByteCount, bool bIsBigEndian)
{
    QString sResult;
    sResult.reserve(nByteCount * 2);

    for (qint32 i = 0; i < nByteCount; i++) {
        const qint32 nByteIndex = bIsBigEndian ? (nByteCount - i - 1) : i;
        const quint8 nByte = (quint8)((nValue >> (nByteIndex * 8)) & 0xFF);
        sResult.append(QString("%1").arg(nByte, 2, 16, QChar('0')));
    }

    return sResult;
}

bool isOneByteType(XBinary::VT valueType)
{
    return (valueType == XBinary::VT_BYTE) || (valueType == XBinary::VT_CHAR) || (valueType == XBinary::VT_UCHAR);
}

bool isHexValueType(XBinary::VT valueType)
{
    return (valueType == XBinary::VT_BYTE) || (valueType == XBinary::VT_WORD) || (valueType == XBinary::VT_DWORD) || (valueType == XBinary::VT_QWORD);
}

void setError(QString *pError, const QString &sError)
{
    if (pError) {
        *pError = sError;
    }
}
}  // namespace

DialogSearch::DialogSearch(QWidget *pParent, QIODevice *pDevice, XBinary::SEARCHDATA *pSearchData, XBinary::SEARCHMODE searchMode, const OPTIONS &options)
    : XShortcutsDialog(pParent, false), ui(new Ui::DialogSearch), m_pSearchData(pSearchData)
{
    Q_UNUSED(pDevice)

    ui->setupUi(this);

    ui->comboBoxSearchFrom->addItem(tr("Beginning"), (quint32)XBinary::SF_BEGIN);
    ui->comboBoxSearchFrom->addItem(tr("Current cursor"), (quint32)XBinary::SF_CURRENTOFFSET);

    ui->comboBoxEncoding->addItem(tr("ANSI"), (quint32)XBinary::VT_A);
    ui->comboBoxEncoding->addItem(tr("UTF-16 LE"), (quint32)XBinary::VT_U);
    ui->comboBoxEncoding->addItem(tr("UTF-8"), (quint32)XBinary::VT_UTF8);

    ui->comboBoxValueType->addItem(tr("Hexadecimal byte (8-bit)"), (quint32)XBinary::VT_BYTE);
    ui->comboBoxValueType->addItem(tr("Hexadecimal word (16-bit)"), (quint32)XBinary::VT_WORD);
    ui->comboBoxValueType->addItem(tr("Hexadecimal dword (32-bit)"), (quint32)XBinary::VT_DWORD);
    ui->comboBoxValueType->addItem(tr("Hexadecimal qword (64-bit)"), (quint32)XBinary::VT_QWORD);
    ui->comboBoxValueType->addItem(tr("Signed integer (8-bit)"), (quint32)XBinary::VT_CHAR);
    ui->comboBoxValueType->addItem(tr("Unsigned integer (8-bit)"), (quint32)XBinary::VT_UCHAR);
    ui->comboBoxValueType->addItem(tr("Signed integer (16-bit)"), (quint32)XBinary::VT_SHORT);
    ui->comboBoxValueType->addItem(tr("Unsigned integer (16-bit)"), (quint32)XBinary::VT_USHORT);
    ui->comboBoxValueType->addItem(tr("Signed integer (32-bit)"), (quint32)XBinary::VT_INT);
    ui->comboBoxValueType->addItem(tr("Unsigned integer (32-bit)"), (quint32)XBinary::VT_UINT);
    ui->comboBoxValueType->addItem(tr("Signed integer (64-bit)"), (quint32)XBinary::VT_INT64);
    ui->comboBoxValueType->addItem(tr("Unsigned integer (64-bit)"), (quint32)XBinary::VT_UINT64);
    ui->comboBoxValueType->addItem(tr("Floating point (32-bit)"), (quint32)XBinary::VT_FLOAT);
    ui->comboBoxValueType->addItem(tr("Floating point (64-bit)"), (quint32)XBinary::VT_DOUBLE);

    XFormats::setEndiannessComboBox(ui->comboBoxEndianness, XBinary::ENDIAN_LITTLE);

    ui->comboBoxValueType->setCurrentIndex(ui->comboBoxValueType->findData((quint32)XBinary::VT_DWORD));
    ui->lineEditValue->setText(QStringLiteral("0"));
    ui->widgetSearchFrom->setVisible(options.bShowBegin);

    qint32 nCurrentTab = 0;
    if ((searchMode >= XBinary::SEARCHMODE_STRING) && (searchMode <= XBinary::SEARCHMODE_VALUE)) {
        nCurrentTab = (qint32)searchMode;
    }
    ui->tabWidgetSearch->setCurrentIndex(nCurrentTab);

    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    ui->lineEditSignature->setFont(fixedFont);
    ui->lineEditValue->setFont(fixedFont);
    ui->lineEditHex->setFont(fixedFont);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DialogSearch::acceptSearch);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &DialogSearch::reject);
    connect(ui->tabWidgetSearch, &QTabWidget::currentChanged, this, [this](qint32) { updateModeState(); });
    connect(ui->lineEditString, &QLineEdit::textChanged, this, &DialogSearch::checkValid);
    connect(ui->lineEditSignature, &QLineEdit::textChanged, this, &DialogSearch::checkValid);
    connect(ui->lineEditValue, &QLineEdit::textChanged, this, &DialogSearch::updateValue);
    connect(ui->comboBoxValueType, static_cast<void (QComboBox::*)(qint32)>(&QComboBox::currentIndexChanged), this, [this](qint32) { updateValue(); });
    connect(ui->comboBoxEndianness, static_cast<void (QComboBox::*)(qint32)>(&QComboBox::currentIndexChanged), this, [this](qint32) { updateValue(); });

    const auto acceptIfValid = [this]() {
        QPushButton *pFindButton = ui->buttonBox->button(QDialogButtonBox::Ok);
        if (pFindButton && pFindButton->isEnabled()) {
            acceptSearch();
        }
    };
    connect(ui->lineEditString, &QLineEdit::returnPressed, this, acceptIfValid);
    connect(ui->lineEditSignature, &QLineEdit::returnPressed, this, acceptIfValid);
    connect(ui->lineEditValue, &QLineEdit::returnPressed, this, acceptIfValid);

    QPushButton *pFindButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    if (pFindButton) {
        pFindButton->setText(tr("Find"));
        pFindButton->setDefault(true);
        pFindButton->setAutoDefault(true);
    }

    updateModeState();
    adjustView();
}

DialogSearch::~DialogSearch()
{
    delete ui;
}

void DialogSearch::adjustView()
{
    setMinimumWidth(qMax(520, minimumSizeHint().width()));
    adjustSize();
}

void DialogSearch::acceptSearch()
{
    checkValid();

    QPushButton *pFindButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    if (!m_pSearchData || !pFindButton || !pFindButton->isEnabled()) {
        return;
    }

    m_pSearchData->startFrom = (XBinary::SF)ui->comboBoxSearchFrom->currentData(Qt::UserRole).toUInt();
    m_pSearchData->endian = XBinary::ENDIAN_LITTLE;

    const qint32 nCurrentTab = ui->tabWidgetSearch->currentIndex();
    if (nCurrentTab == XBinary::SEARCHMODE_STRING) {
        XBinary::VT valueType = (XBinary::VT)ui->comboBoxEncoding->currentData(Qt::UserRole).toUInt();

        if (!ui->checkBoxMatchCase->isChecked()) {
            if (valueType == XBinary::VT_A) {
                valueType = XBinary::VT_A_I;
            } else if (valueType == XBinary::VT_U) {
                valueType = XBinary::VT_U_I;
            } else if (valueType == XBinary::VT_UTF8) {
                valueType = XBinary::VT_UTF8_I;
            }
        }

        m_pSearchData->valueType = valueType;
        m_pSearchData->varValue = ui->lineEditString->text();
    } else if (nCurrentTab == XBinary::SEARCHMODE_SIGNATURE) {
        m_pSearchData->valueType = XBinary::VT_SIGNATURE;
        m_pSearchData->varValue = ui->lineEditSignature->text();
    } else if (nCurrentTab == XBinary::SEARCHMODE_VALUE) {
        QVariant varValue;
        QString sHex;
        QString sError;

        if (!readValue(&varValue, &sHex, &sError)) {
            setStatus(sError);
            return;
        }

        m_pSearchData->valueType = currentValueType();
        m_pSearchData->endian = (XBinary::ENDIAN)ui->comboBoxEndianness->currentData(Qt::UserRole).toUInt();
        m_pSearchData->varValue = varValue;
    }

    QDialog::accept();
}

void DialogSearch::updateModeState()
{
    const qint32 nCurrentTab = ui->tabWidgetSearch->currentIndex();

    if (nCurrentTab == XBinary::SEARCHMODE_STRING) {
        checkValid();
        ui->lineEditString->setFocus(Qt::OtherFocusReason);
        ui->lineEditString->selectAll();
    } else if (nCurrentTab == XBinary::SEARCHMODE_SIGNATURE) {
        checkValid();
        ui->lineEditSignature->setFocus(Qt::OtherFocusReason);
        ui->lineEditSignature->selectAll();
    } else if (nCurrentTab == XBinary::SEARCHMODE_VALUE) {
        updateValue();
        ui->lineEditValue->setFocus(Qt::OtherFocusReason);
        ui->lineEditValue->selectAll();
    }
}

void DialogSearch::updateValue()
{
    const XBinary::VT valueType = currentValueType();
    const bool bOneByte = isOneByteType(valueType);

    ui->labelEndianness->setEnabled(!bOneByte);
    ui->comboBoxEndianness->setEnabled(!bOneByte);

    if (valueType == XBinary::VT_BYTE) {
        ui->lineEditValue->setPlaceholderText(tr("00 to FF"));
        ui->labelValueHelp->setText(tr("Enter 1 or 2 hexadecimal digits."));
    } else if (valueType == XBinary::VT_WORD) {
        ui->lineEditValue->setPlaceholderText(tr("0000 to FFFF"));
        ui->labelValueHelp->setText(tr("Enter up to 4 hexadecimal digits."));
    } else if (valueType == XBinary::VT_DWORD) {
        ui->lineEditValue->setPlaceholderText(tr("e.g. DEADBEEF"));
        ui->labelValueHelp->setText(tr("Enter up to 8 hexadecimal digits."));
    } else if (valueType == XBinary::VT_QWORD) {
        ui->lineEditValue->setPlaceholderText(tr("e.g. 0123456789ABCDEF"));
        ui->labelValueHelp->setText(tr("Enter up to 16 hexadecimal digits."));
    } else if ((valueType == XBinary::VT_FLOAT) || (valueType == XBinary::VT_DOUBLE)) {
        ui->lineEditValue->setPlaceholderText(tr("e.g. 3.14159"));
        ui->labelValueHelp->setText(tr("Enter a finite decimal number."));
    } else if ((valueType == XBinary::VT_CHAR) || (valueType == XBinary::VT_SHORT) || (valueType == XBinary::VT_INT) || (valueType == XBinary::VT_INT64)) {
        ui->lineEditValue->setPlaceholderText(tr("e.g. -42"));
        ui->labelValueHelp->setText(tr("Enter a signed decimal integer."));
    } else {
        ui->lineEditValue->setPlaceholderText(tr("e.g. 255"));
        ui->labelValueHelp->setText(tr("Enter an unsigned decimal integer."));
    }

    QVariant varValue;
    QString sHex;
    QString sError;
    if (readValue(&varValue, &sHex, &sError)) {
        ui->lineEditHex->setText(formatHexBytes(sHex));
    } else {
        ui->lineEditHex->clear();
    }

    checkValid();
}

void DialogSearch::checkValid()
{
    QPushButton *pFindButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    bool bIsValid = false;
    QString sStatus;

    if (!m_pSearchData) {
        ui->tabWidgetSearch->setEnabled(false);
        ui->widgetSearchFrom->setEnabled(false);
        sStatus = tr("Search settings are unavailable.");
    } else {
        ui->tabWidgetSearch->setEnabled(true);
        ui->widgetSearchFrom->setEnabled(true);

        const qint32 nCurrentTab = ui->tabWidgetSearch->currentIndex();
        if (nCurrentTab == XBinary::SEARCHMODE_STRING) {
            const qint32 nLength = ui->lineEditString->text().size();
            bIsValid = (nLength > 0) && (nLength <= MAX_SEARCH_TEXT_LENGTH);

            if (nLength == 0) {
                sStatus = tr("Enter text to search for.");
            } else if (nLength == MAX_SEARCH_TEXT_LENGTH) {
                sStatus = tr("Ready. The 256-character limit has been reached.");
            } else {
                sStatus = tr("Ready. %1 of 256 characters used.").arg(nLength);
            }
        } else if (nCurrentTab == XBinary::SEARCHMODE_SIGNATURE) {
            const QString sSignature = ui->lineEditSignature->text();
            const qint32 nLength = sSignature.size();

            if (sSignature.isEmpty()) {
                sStatus = tr("Enter a byte signature.");
            } else if (!XBinary::isSignatureValid(sSignature)) {
                sStatus = tr("Use hexadecimal byte pairs and ?? wildcards, for example 4D 5A ?? 00.");
            } else {
                bIsValid = (nLength <= MAX_SEARCH_TEXT_LENGTH);
                sStatus = (nLength == MAX_SEARCH_TEXT_LENGTH) ? tr("Signature is valid. The 256-character limit has been reached.")
                                                              : tr("Signature is valid. %1 of 256 characters used.").arg(nLength);
            }
        } else if (nCurrentTab == XBinary::SEARCHMODE_VALUE) {
            QVariant varValue;
            QString sHex;
            QString sError;
            bIsValid = readValue(&varValue, &sHex, &sError);

            if (bIsValid) {
                const qint32 nBytes = sHex.size() / 2;
                sStatus = (nBytes == 1) ? tr("Ready to search for 1 byte.") : tr("Ready to search for %1 bytes.").arg(nBytes);
            } else {
                sStatus = sError;
            }
        }
    }

    if (pFindButton) {
        pFindButton->setEnabled(bIsValid);
    }

    setStatus(sStatus);
}

XBinary::VT DialogSearch::currentValueType() const
{
    bool bSuccess = false;
    const quint32 nValueType = ui->comboBoxValueType->currentData(Qt::UserRole).toUInt(&bSuccess);

    return bSuccess ? (XBinary::VT)nValueType : XBinary::VT_DWORD;
}

bool DialogSearch::readValue(QVariant *pValue, QString *pHex, QString *pError) const
{
    const QString sValue = ui->lineEditValue->text().trimmed();
    const XBinary::VT valueType = currentValueType();
    const bool bIsBigEndian = ((XBinary::ENDIAN)ui->comboBoxEndianness->currentData(Qt::UserRole).toUInt() == XBinary::ENDIAN_BIG);

    if (pValue) {
        pValue->clear();
    }
    if (pHex) {
        pHex->clear();
    }
    if (pError) {
        pError->clear();
    }

    if (sValue.isEmpty()) {
        setError(pError, isHexValueType(valueType) ? tr("Enter a hexadecimal value.") : tr("Enter a decimal value."));
        return false;
    }

    bool bSuccess = false;

    if (valueType == XBinary::VT_BYTE) {
        const quint64 nValue = sValue.toULongLong(&bSuccess, 16);
        if (!bSuccess || (nValue > 0xFFULL)) {
            setError(pError, tr("Enter a hexadecimal value from 00 to FF."));
            return false;
        }
        if (pValue) {
            *pValue = QVariant::fromValue<qulonglong>(nValue);
        }
        if (pHex) {
            *pHex = valueToByteHex(nValue, 1, bIsBigEndian);
        }
    } else if (valueType == XBinary::VT_WORD) {
        const quint64 nValue = sValue.toULongLong(&bSuccess, 16);
        if (!bSuccess || (nValue > 0xFFFFULL)) {
            setError(pError, tr("Enter a hexadecimal value from 0000 to FFFF."));
            return false;
        }
        if (pValue) {
            *pValue = QVariant::fromValue<qulonglong>(nValue);
        }
        if (pHex) {
            *pHex = valueToByteHex(nValue, 2, bIsBigEndian);
        }
    } else if (valueType == XBinary::VT_DWORD) {
        const quint64 nValue = sValue.toULongLong(&bSuccess, 16);
        if (!bSuccess || (nValue > 0xFFFFFFFFULL)) {
            setError(pError, tr("Enter a hexadecimal value of up to 8 digits."));
            return false;
        }
        if (pValue) {
            *pValue = QVariant::fromValue<qulonglong>(nValue);
        }
        if (pHex) {
            *pHex = valueToByteHex(nValue, 4, bIsBigEndian);
        }
    } else if (valueType == XBinary::VT_QWORD) {
        const quint64 nValue = sValue.toULongLong(&bSuccess, 16);
        if (!bSuccess) {
            setError(pError, tr("Enter a hexadecimal value of up to 16 digits."));
            return false;
        }
        if (pValue) {
            *pValue = QVariant::fromValue<qulonglong>(nValue);
        }
        if (pHex) {
            *pHex = valueToByteHex(nValue, 8, bIsBigEndian);
        }
    } else if (valueType == XBinary::VT_CHAR) {
        const qint64 nValue = sValue.toLongLong(&bSuccess, 10);
        if (!bSuccess || (nValue < -128) || (nValue > 127)) {
            setError(pError, tr("Enter a decimal integer from -128 to 127."));
            return false;
        }
        if (pValue) {
            *pValue = QVariant::fromValue<qlonglong>(nValue);
        }
        if (pHex) {
            *pHex = valueToByteHex((quint8)((qint8)nValue), 1, bIsBigEndian);
        }
    } else if (valueType == XBinary::VT_UCHAR) {
        const quint64 nValue = sValue.toULongLong(&bSuccess, 10);
        if (!bSuccess || (nValue > 255)) {
            setError(pError, tr("Enter a decimal integer from 0 to 255."));
            return false;
        }
        if (pValue) {
            *pValue = QVariant::fromValue<qulonglong>(nValue);
        }
        if (pHex) {
            *pHex = valueToByteHex(nValue, 1, bIsBigEndian);
        }
    } else if (valueType == XBinary::VT_SHORT) {
        const qint64 nValue = sValue.toLongLong(&bSuccess, 10);
        if (!bSuccess || (nValue < (std::numeric_limits<qint16>::min)()) || (nValue > (std::numeric_limits<qint16>::max)())) {
            setError(pError, tr("Enter a decimal integer from -32768 to 32767."));
            return false;
        }
        if (pValue) {
            *pValue = QVariant::fromValue<qlonglong>(nValue);
        }
        if (pHex) {
            *pHex = valueToByteHex((quint16)((qint16)nValue), 2, bIsBigEndian);
        }
    } else if (valueType == XBinary::VT_USHORT) {
        const quint64 nValue = sValue.toULongLong(&bSuccess, 10);
        if (!bSuccess || (nValue > (std::numeric_limits<quint16>::max)())) {
            setError(pError, tr("Enter a decimal integer from 0 to 65535."));
            return false;
        }
        if (pValue) {
            *pValue = QVariant::fromValue<qulonglong>(nValue);
        }
        if (pHex) {
            *pHex = valueToByteHex(nValue, 2, bIsBigEndian);
        }
    } else if (valueType == XBinary::VT_INT) {
        const qint64 nValue = sValue.toLongLong(&bSuccess, 10);
        if (!bSuccess || (nValue < (std::numeric_limits<qint32>::min)()) || (nValue > (std::numeric_limits<qint32>::max)())) {
            setError(pError, tr("Enter a decimal integer from -2147483648 to 2147483647."));
            return false;
        }
        if (pValue) {
            *pValue = QVariant::fromValue<qlonglong>(nValue);
        }
        if (pHex) {
            *pHex = valueToByteHex((quint32)((qint32)nValue), 4, bIsBigEndian);
        }
    } else if (valueType == XBinary::VT_UINT) {
        const quint64 nValue = sValue.toULongLong(&bSuccess, 10);
        if (!bSuccess || (nValue > (std::numeric_limits<quint32>::max)())) {
            setError(pError, tr("Enter a decimal integer from 0 to 4294967295."));
            return false;
        }
        if (pValue) {
            *pValue = QVariant::fromValue<qulonglong>(nValue);
        }
        if (pHex) {
            *pHex = valueToByteHex(nValue, 4, bIsBigEndian);
        }
    } else if (valueType == XBinary::VT_INT64) {
        const qint64 nValue = sValue.toLongLong(&bSuccess, 10);
        if (!bSuccess) {
            setError(pError, tr("Enter a signed 64-bit decimal integer."));
            return false;
        }
        if (pValue) {
            *pValue = QVariant::fromValue<qlonglong>(nValue);
        }
        if (pHex) {
            *pHex = valueToByteHex((quint64)nValue, 8, bIsBigEndian);
        }
    } else if (valueType == XBinary::VT_UINT64) {
        const quint64 nValue = sValue.toULongLong(&bSuccess, 10);
        if (!bSuccess) {
            setError(pError, tr("Enter an unsigned 64-bit decimal integer."));
            return false;
        }
        if (pValue) {
            *pValue = QVariant::fromValue<qulonglong>(nValue);
        }
        if (pHex) {
            *pHex = valueToByteHex(nValue, 8, bIsBigEndian);
        }
    } else if (valueType == XBinary::VT_FLOAT) {
        const float fValue = sValue.toFloat(&bSuccess);
        if (!bSuccess || !qIsFinite(fValue)) {
            setError(pError, tr("Enter a finite 32-bit decimal number."));
            return false;
        }
        if (pValue) {
            *pValue = fValue;
        }
        if (pHex) {
            quint32 nBits = 0;
            static_assert(sizeof(nBits) == sizeof(fValue), "Unexpected float width");
            std::memcpy(&nBits, &fValue, sizeof(nBits));
            *pHex = valueToByteHex(nBits, 4, bIsBigEndian);
        }
    } else if (valueType == XBinary::VT_DOUBLE) {
        const double dValue = sValue.toDouble(&bSuccess);
        if (!bSuccess || !qIsFinite(dValue)) {
            setError(pError, tr("Enter a finite 64-bit decimal number."));
            return false;
        }
        if (pValue) {
            *pValue = dValue;
        }
        if (pHex) {
            quint64 nBits = 0;
            static_assert(sizeof(nBits) == sizeof(dValue), "Unexpected double width");
            std::memcpy(&nBits, &dValue, sizeof(nBits));
            *pHex = valueToByteHex(nBits, 8, bIsBigEndian);
        }
    } else {
        setError(pError, tr("Select a supported value type."));
        return false;
    }

    return true;
}

void DialogSearch::setStatus(const QString &sText)
{
    ui->labelStatus->setText(sText);
}

void DialogSearch::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
}
