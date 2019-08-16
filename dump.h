#ifndef DUMP_H
#define DUMP_H

#include <QObject>
#include <QElapsedTimer>
#include <QFile>

class Dump : public QObject
{
    Q_OBJECT

public:
    explicit Dump(QObject *parent=nullptr);
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

#endif // DUMP_H
