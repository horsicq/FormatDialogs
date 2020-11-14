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
#include "searchprocess.h"

SearchProcess::SearchProcess(QObject *pParent) : QObject(pParent)
{
    connect(&g_binary,SIGNAL(findProgressValueChanged(qint32)),this,SIGNAL(progressValueChanged(qint32)));
    connect(&g_binary,SIGNAL(findProgressMinimumChanged(qint32)),this,SIGNAL(progressValueMinimum(qint32)));
    connect(&g_binary,SIGNAL(findProgressMaximumChanged(qint32)),this,SIGNAL(progressValueMaximum(qint32)));
}

void SearchProcess::setData(QIODevice *pDevice, SearchProcess::SEARCHDATA *pSearchData)
{
    this->g_pDevice=pDevice;
    this->g_pSearchData=pSearchData;
}

void SearchProcess::stop()
{
    g_binary.setDumpProcessEnable(false);
}

void SearchProcess::process()
{
    QElapsedTimer scanTimer;
    scanTimer.start();

    g_binary.setDevice(g_pDevice);

    qint64 nStartOffset=0;

    if(g_pSearchData->startFrom==SF_CURRENTOFFSET)
    {
        nStartOffset=g_pSearchData->nCurrentOffset;
    }

    if(g_pSearchData->type==TYPE_ANSISTRING)
    {
        g_pSearchData->nResult=g_binary.find_ansiString(nStartOffset,-1,g_pSearchData->variant.toString());
    }
    else if(g_pSearchData->type==TYPE_ANSISTRING_I)
    {
        g_pSearchData->nResult=g_binary.find_ansiStringI(nStartOffset,-1,g_pSearchData->variant.toString());
    }
    else if(g_pSearchData->type==TYPE_UNICODESTRING)
    {
        g_pSearchData->nResult=g_binary.find_unicodeString(nStartOffset,-1,g_pSearchData->variant.toString());
    }
    else if(g_pSearchData->type==TYPE_UNICODESTRING_I)
    {
        g_pSearchData->nResult=g_binary.find_unicodeStringI(nStartOffset,-1,g_pSearchData->variant.toString());
    }
    else if(g_pSearchData->type==TYPE_SIGNATURE)
    {
        g_pSearchData->nResult=g_binary.find_signature(nStartOffset,-1,g_pSearchData->variant.toString());
    }
    else if(g_pSearchData->type==TYPE_VALUE_CHAR)
    {
        g_pSearchData->nResult=g_binary.find_int8(nStartOffset,-1,(qint8)(g_pSearchData->variant.toULongLong()));
    }
    else if(g_pSearchData->type==TYPE_VALUE_UCHAR)
    {
        g_pSearchData->nResult=g_binary.find_uint8(nStartOffset,-1,(quint8)(g_pSearchData->variant.toULongLong()));
    }
    else if(g_pSearchData->type==TYPE_VALUE_SHORT)
    {
        g_pSearchData->nResult=g_binary.find_int16(nStartOffset,-1,(qint16)(g_pSearchData->variant.toULongLong()),g_pSearchData->bIsBigEndian);
    }
    else if(g_pSearchData->type==TYPE_VALUE_USHORT)
    {
        g_pSearchData->nResult=g_binary.find_uint16(nStartOffset,-1,(quint16)(g_pSearchData->variant.toULongLong()),g_pSearchData->bIsBigEndian);
    }
    else if(g_pSearchData->type==TYPE_VALUE_INT)
    {
        g_pSearchData->nResult=g_binary.find_int32(nStartOffset,-1,(qint32)(g_pSearchData->variant.toULongLong()),g_pSearchData->bIsBigEndian);
    }
    else if(g_pSearchData->type==TYPE_VALUE_UINT)
    {
        g_pSearchData->nResult=g_binary.find_uint32(nStartOffset,-1,(quint32)(g_pSearchData->variant.toULongLong()),g_pSearchData->bIsBigEndian);
    }
    else if(g_pSearchData->type==TYPE_VALUE_INT64)
    {
        g_pSearchData->nResult=g_binary.find_int64(nStartOffset,-1,(qint64)(g_pSearchData->variant.toULongLong()),g_pSearchData->bIsBigEndian);
    }
    else if(g_pSearchData->type==TYPE_VALUE_UINT64)
    {
        g_pSearchData->nResult=g_binary.find_uint64(nStartOffset,-1,(quint64)(g_pSearchData->variant.toULongLong()),g_pSearchData->bIsBigEndian);
    }
    else if(g_pSearchData->type==TYPE_VALUE_FLOAT)
    {
        g_pSearchData->nResult=g_binary.find_float(nStartOffset,-1,(float)(g_pSearchData->variant.toFloat()),g_pSearchData->bIsBigEndian);
    }
    else if(g_pSearchData->type==TYPE_VALUE_DOUBLE)
    {
        g_pSearchData->nResult=g_binary.find_double(nStartOffset,-1,(double)(g_pSearchData->variant.toDouble()),g_pSearchData->bIsBigEndian);
    }

    if(g_pSearchData->nResult!=-1)
    {
        g_pSearchData->bInit=true;
    }

    emit completed(scanTimer.elapsed());
}
