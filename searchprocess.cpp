/* Copyright (c) 2019-2025 hors<horsicq@gmail.com>
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

SearchProcess::SearchProcess(QObject *pParent) : XThreadObject(pParent)
{
    m_pDevice = nullptr;
    m_pSearchData = nullptr;
    m_pPdStruct = nullptr;
}

void SearchProcess::setData(QIODevice *pDevice, XBinary::SEARCHDATA *pSearchData, XBinary::PDSTRUCT *pPdStruct)
{
    this->m_pDevice = pDevice;
    this->m_pSearchData = pSearchData;
    this->m_pPdStruct = pPdStruct;
}

void SearchProcess::process()
{
    qint32 _nFreeIndex = XBinary::getFreeIndex(m_pPdStruct);
    XBinary::setPdStructInit(m_pPdStruct, _nFreeIndex, 0);

    XBinary binary(m_pDevice);

    connect(&binary, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));

    qint64 nStartOffset = 0;

    if (m_pSearchData->startFrom == XBinary::SF_CURRENTOFFSET) {
        nStartOffset = m_pSearchData->nCurrentOffset;
    }

    XBinary::_MEMORY_MAP memoryMap = binary.getMemoryMap(XBinary::MAPMODE_UNKNOWN, m_pPdStruct);

    bool bIsBigEndian = (m_pSearchData->endian == XBinary::ENDIAN_BIG);

    m_pSearchData->nResultOffset =
        binary.find_value(&memoryMap, nStartOffset, -1, m_pSearchData->varValue, m_pSearchData->valueType, bIsBigEndian, &(m_pSearchData->nResultSize), m_pPdStruct);

    if (m_pSearchData->nResultOffset != -1) {
        m_pSearchData->bIsInit = true;
    }

    XBinary::setPdStructFinished(m_pPdStruct, _nFreeIndex);
}
