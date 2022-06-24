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
#include "dumpprocess.h"

DumpProcess::DumpProcess(QObject *pParent) : QObject(pParent)
{
    g_pPdStruct=nullptr;
}

void DumpProcess::setData(QIODevice *pDevice, qint64 nOffset, qint64 nSize, QString sFileName, DT dumpType, XBinary::PDSTRUCT *pPdStruct)
{
    this->g_pDevice=pDevice;
    this->g_nOffset=nOffset;
    this->g_nSize=nSize;
    this->g_sFileName=sFileName;
    this->g_dumpType=dumpType;
    this->g_pPdStruct=pPdStruct;
}

void DumpProcess::process()
{
    QElapsedTimer scanTimer;
    scanTimer.start();

    XBinary binary(g_pDevice);

    connect(&binary,SIGNAL(errorMessage(QString)),this,SIGNAL(errorMessage(QString)));

    if(g_dumpType==DT_OFFSET)
    {
        binary.dumpToFile(g_sFileName,g_nOffset,g_nSize,g_pPdStruct);
    }

    emit completed(scanTimer.elapsed());
}
