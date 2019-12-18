#ifndef DUMPPROCESS_H
#define DUMPPROCESS_H

#include <QObject>
#include <QElapsedTimer>
#include <QFile>

class DumpProcess : public QObject
{
    Q_OBJECT

public:
    explicit DumpProcess(QObject *parent=nullptr);
    void setData(QIODevice *pDevice, qint64 nOffset, qint64 nSize, QString sFileName);

signals:
    void errorMessage(QString sText);
    void completed(qint64 nElapsed);
    void progressValue(qint32 nValue);

public slots:
    void stop();
    void process();

private:
    QIODevice *pDevice;
    qint64 nOffset;
    qint64 nSize;
    QString sFileName;
    bool bIsStop;
};

#endif // DUMPPROCESS_H
