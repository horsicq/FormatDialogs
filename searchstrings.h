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
#ifndef SEARCHSTRINGS_H
#define SEARCHSTRINGS_H

#include <QObject>
#include <QIODevice>
#include <QElapsedTimer>
#include <QStandardItemModel>

class SearchStrings : public QObject
{
    Q_OBJECT

public:
    enum RECORD_TYPE
    {
        RECORD_TYPE_UNKNOWN=0,
        RECORD_TYPE_ANSI,
        RECORD_TYPE_UNICODE
    };

    struct RECORD
    {
        QString sString;
        qint64 nOffset;
        qint64 nSize;
        RECORD_TYPE recordType;
    };

    struct OPTIONS
    {
        qint64 nBaseAddress;
        qint32 nAddressWidth;
        bool bSearchAnsi;
        bool bSearchUnicode;
    };

    const int N_MAX=50000;

    explicit SearchStrings(QObject *parent=nullptr);
    void setSearchData(QIODevice *pDevice,QList<RECORD> *pListRecords, OPTIONS *pOptions=nullptr);
    void setModelData(QList<SearchStrings::RECORD> *pListRecords, QStandardItemModel **ppModel, OPTIONS *pOptions=nullptr);

signals:
    void errorMessage(QString sText);
    void completed(qint64 nElapsed);
    void progressValue(qint32 nValue);

public slots:
    void stop();
    void processSearch();
    void processModel();

private:
    bool isAnsiSymbol(quint8 cCode);
    bool isUnicodeSymbol(quint16 nCode);

private:
    QIODevice *pDevice;
    QList<RECORD> *pListRecords;
    OPTIONS options;
    QStandardItemModel **ppModel;
    bool bIsStop;
};

#endif // SEARCHSTRINGS_H
