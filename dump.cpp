#include "dump.h"

Dump::Dump(QObject *parent) : QObject(parent)
{
    bIsStop=false;
}

void Dump::setData(QIODevice *pDevice, qint64 nOffset, qint64 nSize, QString sFileName)
{
    this->pDevice=pDevice;
    this->nOffset=nOffset;
    this->nSize=nSize;
    this->sFileName=sFileName;
}

void Dump::stop()
{
    bIsStop=true;
}

void Dump::process()
{
    const qint64 N_BUFFER_SIZE=0x1000;

    qint64 _nProcent=nSize/100;

    QElapsedTimer scanTimer;
    scanTimer.start();

    QFile file;
    file.setFileName(sFileName);

    if(file.open(QIODevice::ReadWrite))
    {
        bool bReadError=false;
        bool bWriteError=false;

        qint64 _nOffset=this->nOffset;
        qint64 _nFileOffset=0;
        qint64 _nSize=this->nSize;
        qint32 _nCurrentProcent=0;

        char *pBuffer=new char[0x1000];
        while(_nSize>0)
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

            if(file.seek(_nFileOffset))
            {
                if(file.write(pBuffer,nCurrentSize)!=nCurrentSize)
                {
                    bWriteError=true;
                    break;
                }
            }
            else
            {
                bWriteError=true;
                break;
            }

            _nSize-=nCurrentSize;
            _nOffset+=nCurrentSize;
            _nFileOffset+=nCurrentSize;

            if(_nFileOffset>((_nCurrentProcent+1)*_nProcent))
            {
                _nCurrentProcent++;
                emit progressBarValue(_nCurrentProcent);
            }
        }

        delete [] pBuffer;

        file.close();

        if(bReadError)
        {
            emit errorMessage(tr("Read error"));
        }
        if(bWriteError)
        {
            emit errorMessage(tr("Write error"));
        }
    }
    else
    {
        emit errorMessage(tr("Cannot open file")+QString(": %1").arg(sFileName));
    }

    emit completed(scanTimer.elapsed());
}
