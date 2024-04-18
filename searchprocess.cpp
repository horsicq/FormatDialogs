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
#include "searchprocess.h"

SearchProcess::SearchProcess(QObject *pParent) : QObject(pParent)
{
    g_pDevice = nullptr;
    g_pSearchData = nullptr;
    g_pPdStruct = nullptr;
}

void SearchProcess::setData(QIODevice *pDevice, XBinary::SEARCHDATA *pSearchData, XBinary::PDSTRUCT *pPdStruct)
{
    this->g_pDevice = pDevice;
    this->g_pSearchData = pSearchData;
    this->g_pPdStruct = pPdStruct;
}

void SearchProcess::process()
{
    QElapsedTimer scanTimer;
    scanTimer.start();

    qint32 _nFreeIndex = XBinary::getFreeIndex(g_pPdStruct);
    XBinary::setPdStructInit(g_pPdStruct, _nFreeIndex, 0);

    XBinary binary(g_pDevice);

    connect(&binary, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));

    qint64 nStartOffset = 0;

    if (g_pSearchData->startFrom == XBinary::SF_CURRENTOFFSET) {
        nStartOffset = g_pSearchData->nCurrentOffset;
    }

    XBinary::_MEMORY_MAP memoryMap = binary.getMemoryMap(XBinary::MAPMODE_UNKNOWN, g_pPdStruct);

    bool bIsBigEndian = (g_pSearchData->endian == XBinary::ENDIAN_BIG);

    g_pSearchData->nResultOffset = binary.find_value(&memoryMap, nStartOffset, -1, g_pSearchData->varValue, g_pSearchData->valueType, bIsBigEndian,
                                                     &(g_pSearchData->nResultSize), g_pPdStruct);

    if (g_pSearchData->nResultOffset != -1) {
        g_pSearchData->bInit = true;
    }

    XBinary::setPdStructFinished(g_pPdStruct, _nFreeIndex);

    emit completed(scanTimer.elapsed());
}
