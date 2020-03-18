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
#ifndef SEARCHPROCESS_H
#define SEARCHPROCESS_H

#include <QObject>
#include <QVariant>
#include "xbinary.h"

class SearchProcess : public QObject
{
    Q_OBJECT

public:
    enum ST
    {
        ST_UNKNOWN=0,
        ST_ANSISTRING,
        ST_UNICODESTRING,
        ST_SIGNATURE,
        // TODO more
    };

    enum SF
    {
        SF_BEGIN=0,
        SF_CURRENTOFFSET
    };

    struct SEARCHDATA
    {
        qint64 nCurrentOffset;
        SF startFrom;
        ST status;
        QVariant variant;
    };

    explicit SearchProcess(QObject *parent);
    void setData(QIODevice *pDevice, SearchProcess::SEARCHDATA *pSearchData);

signals:
    void errorMessage(QString sText);
    void completed(qint64 nElapsed);
    void progressValueChanged(qint32 nValue);
    void progressValueMaximum(qint32 nValue);

public slots:
    void stop();
    void process();

private:
    XBinary binary;
    QIODevice *pDevice;
    SearchProcess::SEARCHDATA *pSearchData;
};

#endif // SEARCHPROCESS_H
