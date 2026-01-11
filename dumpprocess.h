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
#ifndef DUMPPROCESS_H
#define DUMPPROCESS_H

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "xbinary.h"
#include "xthreadobject.h"
#ifdef USE_XPROCESS
#include "xprocess.h"
#endif

class DumpProcess : public XThreadObject {
    Q_OBJECT

public:
    enum DT {
        DT_DUMP_DEVICE_OFFSET = 0,
        DT_PATCH_DEVICE_OFFSET,
#ifdef USE_XPROCESS
        DT_DUMP_PROCESS_USER_READPROCESSMEMORY_RAWDUMP,  // Windows
        DT_DUMP_PROCESS_USER_READPROCESSMEMORY_REBUILD,  // Windows
        DT_DUMP_PROCESS_USER_PROCPIDMEM_RAWDUMP,         // Linux
        DT_DUMP_PROCESS_USER_PTRACE_RAWDUMP,             // Linux
        DT_DUMP_PROCESS_USER_PROCPIDMEM_REBUILD,         // Linux
        DT_DUMP_PROCESS_USER_PTRACE_REBUILD,             // Linux
#endif
    };

    struct RECORD {
        qint64 nOffset;
        qint64 nSize;
        QString sFileName;
    };

    explicit DumpProcess(QObject *pParent = nullptr);

    void setData(QIODevice *pDevice, QList<RECORD> listRecords, DT dumpType, const QString &sJsonFileName, XBinary::PDSTRUCT *pPdStruct);
    void setData(QIODevice *pDevice, DT dumpType, const QString &sJsonFileName, XBinary::PDSTRUCT *pPdStruct);
    void setData(QIODevice *pDevice, qint64 nOffset, qint64 nSize, const QString &sFileName, DumpProcess::DT dumpType, XBinary::PDSTRUCT *pPdStruct);
    void setData(QIODevice *pDevice, DumpProcess::RECORD record, DumpProcess::DT dumpType, XBinary::PDSTRUCT *pPdStruct);
#ifdef USE_XPROCESS
    void setData(X_ID nProcessID, XADDR nAddress, qint64 nSize, DT dumpType, const QString &sFileName, const QString &sJsonFileName, XBinary::PDSTRUCT *pPdStruct);
#ifdef Q_OS_WIN
    void setData(X_ID nProcessID, XADDR nAddress, qint64 nSize, DT dumpType, const QString &sFileName, const QString &sJsonFileName,
                 const XPE::FIXDUMP_OPTIONS &fixDumpOptions, const QByteArray &baHeaders, XBinary::PDSTRUCT *pPdStruct);
#endif
#ifdef Q_OS_LINUX
    void setData(X_ID nProcessID, XADDR nAddress, qint64 nSize, DT dumpType, const QString &sFileName, const QString &sJsonFileName,
                 const XELF::FIXDUMP_OPTIONS &fixDumpOptions, const QByteArray &baHeaders, XBinary::PDSTRUCT *pPdStruct);
#endif
#endif
    void process();

private:
    QIODevice *m_pDevice;
    QList<RECORD> m_listRecords;
    DT m_dumpType;
    QString m_sFileName;
    QString m_sJsonFileName;
    XBinary::PDSTRUCT *m_pPdStruct;
#ifdef USE_XPROCESS
    X_ID m_nProcessID;
    XADDR m_nAddress;
    qint64 m_nSize;
    QByteArray m_baHeaders;
#ifdef Q_OS_WIN
    XPE::FIXDUMP_OPTIONS m_fixDumpOptions;
#endif
#ifdef Q_OS_LINUX
    XELF::FIXDUMP_OPTIONS m_fixDumpOptions;
#endif
#endif
};

#endif  // DUMPPROCESS_H
