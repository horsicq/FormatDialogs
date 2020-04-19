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
#include "searchstrings.h"

SearchStrings::SearchStrings(QObject *parent) : QObject(parent)
{
    bIsStop=false;
    options={};
    ppModel=nullptr;
}

void SearchStrings::setSearchData(QIODevice *pDevice, QList<RECORD> *pListRecords, OPTIONS *pOptions)
{
    this->pDevice=pDevice;
    this->pListRecords=pListRecords;

    if(pOptions)
    {
        options=*pOptions;
    }
}

void SearchStrings::setModelData(QList<SearchStrings::RECORD> *pListRecords, QStandardItemModel **ppModel, OPTIONS *pOptions)
{
    this->pListRecords=pListRecords;
    this->ppModel=ppModel;

    if(pOptions)
    {
        options=*pOptions;
    }
}

void SearchStrings::stop()
{
    bIsStop=true;
}

void SearchStrings::processSearch()
{
    QElapsedTimer scanTimer;
    scanTimer.start();

    pListRecords->clear();

    qint64 nBaseAddress=options.nBaseAddress;

    const qint64 N_BUFFER_SIZE=0x1000;
    const qint64 N_MAX_STRING_SIZE=128;

    qint64 _nSize=pDevice->size();
    qint64 _nOffset=0;
    qint64 _nRawOffset=0;
    qint64 _nProcent=_nSize/100;
    qint32 _nCurrentProcent=0;

    bool bReadError=false;

    char *pBuffer=new char[N_BUFFER_SIZE];
    char *pAnsiBuffer=new char[N_MAX_STRING_SIZE+1];

    quint16 *pUnicodeBuffer[2]={new quint16[N_MAX_STRING_SIZE+1],new quint16[N_MAX_STRING_SIZE+1]};
    qint64 nCurrentUnicodeSize[2]={0,0};
    qint64 nCurrentUnicodeOffset[2]={0,0};

    qint64 nCurrentAnsiSize=0;
    qint64 nCurrentAnsiOffset=0;

    bool bIsStart=true;
    char cPrevSymbol=0;

    emit progressValue(_nCurrentProcent);

    bIsStop=false;

    int nCurrentRecords=0;

    while((_nSize>0)&&(!bIsStop))
    {
        qint64 nCurrentSize=qMin(N_BUFFER_SIZE,_nSize);

        if(pDevice->seek(_nOffset))
        {
            if(pDevice->read(pBuffer,nCurrentSize)!=nCurrentSize)
            {
                bReadError=true;
                break;
            }
        }
        else
        {
            bReadError=true;
            break;
        }

        for(qint64 i=0; i<nCurrentSize; i++)
        {
            bool bIsEnd=((i==(nCurrentSize-1))&&(_nSize==nCurrentSize));
            int nParity=(_nOffset+i)%2;

            char cSymbol=*(pBuffer+i);

            bool bIsAnsiSymbol=isAnsiSymbol((quint8)cSymbol);

            if(bIsAnsiSymbol)
            {
                if(nCurrentAnsiSize==0)
                {
                    nCurrentAnsiOffset=_nOffset+i;
                }

                if(nCurrentAnsiSize<N_MAX_STRING_SIZE)
                {
                    *(pAnsiBuffer+nCurrentAnsiSize)=cSymbol;
                }

                nCurrentAnsiSize++;
            }

            if((!bIsAnsiSymbol)||(bIsEnd))
            {
                if(nCurrentAnsiSize>4)
                {
                    if(nCurrentAnsiSize-1<N_MAX_STRING_SIZE)
                    {
                        pAnsiBuffer[nCurrentAnsiSize]=0;
                    }
                    else
                    {
                        pAnsiBuffer[N_MAX_STRING_SIZE]=0;
                    }

                    if(options.bSearchAnsi)
                    {
                        RECORD record;
                        record.recordType=RECORD_TYPE_ANSI;
                        record.nOffset=nCurrentAnsiOffset+nBaseAddress;
                        record.nSize=nCurrentAnsiSize;
                        record.sString=pAnsiBuffer;

                        pListRecords->append(record);

                        nCurrentRecords++;

                        if(nCurrentRecords>=N_MAX)
                        {
                            break;
                        }
                    }
                }

                nCurrentAnsiSize=0;
            }

            if(!bIsStart)
            {
                quint16 nCode=cPrevSymbol+(cSymbol<<8); // TODO BE/LE

                bool bIsUnicodeSymbol=isUnicodeSymbol(nCode);

                if(bIsUnicodeSymbol)
                {
                    if(nCurrentUnicodeSize[nParity]==0)
                    {
                        nCurrentUnicodeOffset[nParity]=_nOffset-1+i;
                    }

                    if(nCurrentUnicodeSize[nParity]<N_MAX_STRING_SIZE)
                    {
                        *(pUnicodeBuffer[nParity]+nCurrentUnicodeSize[nParity])=nCode;
                    }

                    nCurrentUnicodeSize[nParity]++;
                }

                if((!bIsUnicodeSymbol)||(bIsEnd))
                {
                    if(nCurrentUnicodeSize[nParity]>4)
                    {
                        if(nCurrentUnicodeSize[nParity]-1<N_MAX_STRING_SIZE)
                        {
                            pUnicodeBuffer[nParity][nCurrentUnicodeSize[nParity]]=0;
                        }
                        else
                        {
                            pUnicodeBuffer[nParity][N_MAX_STRING_SIZE]=0;
                        }

                        if(options.bSearchUnicode)
                        {
                            RECORD record;
                            record.recordType=RECORD_TYPE_UNICODE;
                            record.nOffset=nCurrentUnicodeOffset[nParity]+nBaseAddress;
                            record.nSize=nCurrentUnicodeSize[nParity];
                            record.sString=QString::fromUtf16(pUnicodeBuffer[nParity]);

                            pListRecords->append(record);

                            nCurrentRecords++;

                            if(nCurrentRecords>=N_MAX)
                            {
                                break;
                            }
                        }
                    }

                    if(bIsEnd)
                    {
                        int nO=(nParity==1)?(0):(1);

                        if(nCurrentUnicodeSize[nO]>4)
                        {
                            if(nCurrentUnicodeSize[nO]-1<N_MAX_STRING_SIZE)
                            {
                                pUnicodeBuffer[nO][nCurrentUnicodeSize[nO]]=0;
                            }
                            else
                            {
                                pUnicodeBuffer[nO][N_MAX_STRING_SIZE]=0;
                            }

                            if(options.bSearchUnicode)
                            {
                                RECORD record;
                                record.recordType=RECORD_TYPE_UNICODE;
                                record.nOffset=nCurrentUnicodeOffset[nO]+nBaseAddress;
                                record.nSize=nCurrentUnicodeSize[nO];
                                record.sString=QString::fromUtf16(pUnicodeBuffer[nO]);

                                pListRecords->append(record);

                                nCurrentRecords++;

                                if(nCurrentRecords>=N_MAX)
                                {
                                    break;
                                }
                            }
                        }
                    }

                    nCurrentUnicodeSize[nParity]=0;
                }
            }

            cPrevSymbol=cSymbol;

            if(bIsStart)
            {
                bIsStart=false;
            }
        }

        _nSize-=nCurrentSize;
        _nOffset+=nCurrentSize;
        _nRawOffset+=nCurrentSize;

        if(_nRawOffset>((_nCurrentProcent+1)*_nProcent))
        {
            _nCurrentProcent++;
            emit progressValue(_nCurrentProcent);
        }

        if(nCurrentRecords>=N_MAX)
        {
            emit errorMessage(QString("%1: %2").arg(tr("Maximum")).arg(nCurrentRecords));

            break;
        }
    }

    if(bReadError)
    {
        emit errorMessage(tr("Read error"));
    }

    bIsStop=false;

    delete [] pBuffer;
    delete [] pAnsiBuffer;
    delete [] pUnicodeBuffer[0];
    delete [] pUnicodeBuffer[1];

    emit completed(scanTimer.elapsed());
}

void SearchStrings::processModel()
{
    QElapsedTimer scanTimer;
    scanTimer.start();

    int nCount=pListRecords->count();
    *ppModel=new QStandardItemModel(nCount,4); // TODO Check maximum

    qint64 nBaseAddress=options.nBaseAddress;
    qint32 nAddressWidth=options.nAddressWidth;

    qint64 _nProcent=nCount/100;
    qint32 _nCurrentProcent=0;

    emit progressValue(_nCurrentProcent);

    (*ppModel)->setHeaderData(0,Qt::Horizontal,nBaseAddress?(tr("Address")):(tr("Offset")));
    (*ppModel)->setHeaderData(1,Qt::Horizontal,tr("Size"));
    (*ppModel)->setHeaderData(2,Qt::Horizontal,tr("Type"));
    (*ppModel)->setHeaderData(3,Qt::Horizontal,tr("String"));

    emit progressValue(0);

    bIsStop=false;

    for(int i=0; (i<nCount)&&(!bIsStop); i++)
    {
        SearchStrings::RECORD record=pListRecords->at(i);

        QStandardItem *pTypeAddress=new QStandardItem;
        pTypeAddress->setText(QString("%1").arg(record.nOffset,nAddressWidth,16,QChar('0')));
        pTypeAddress->setTextAlignment(Qt::AlignRight);
        (*ppModel)->setItem(i,0,pTypeAddress);

        QStandardItem *pTypeSize=new QStandardItem;
        pTypeSize->setText(QString("%1").arg(record.nSize,8,16,QChar('0')));
        pTypeSize->setTextAlignment(Qt::AlignRight);
        (*ppModel)->setItem(i,1,pTypeSize);

        QStandardItem *pTypeItem=new QStandardItem;

        if(record.recordType==SearchStrings::RECORD_TYPE_ANSI)
        {
            pTypeItem->setText("A");
        }
        else if(record.recordType==SearchStrings::RECORD_TYPE_UNICODE)
        {
            pTypeItem->setText("U");
        }

        (*ppModel)->setItem(i,2,pTypeItem);
        (*ppModel)->setItem(i,3,new QStandardItem(record.sString));

        if(i>((_nCurrentProcent+1)*_nProcent))
        {
            _nCurrentProcent++;
            emit progressValue(_nCurrentProcent);
        }
    }

    bIsStop=false;

    emit completed(scanTimer.elapsed());
}

bool SearchStrings::isAnsiSymbol(quint8 cCode)
{
    bool bResult=false;

    if((cCode>=20)&&(cCode<0x80))
    {
        bResult=true;
    }

    return bResult;
}

bool SearchStrings::isUnicodeSymbol(quint16 nCode)
{
    bool bResult=false;

    if((nCode>=20)&&(nCode<0x80))
    {
        bResult=true;
    }

    return bResult;
}
