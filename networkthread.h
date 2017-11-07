#ifndef NETWORKTHREAD_H
#define NETWORKTHREAD_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QTcpServer>
class NetworkThread : public QThread
{
    Q_OBJECT
    void run() override {
        QString result = "OK";
        while(1){
            qDebug()<<"here is new thread"<<endl;
            QThread::msleep(1000);
        }
        emit resultReady(result);
    }
public:
    NetworkThread();
signals:
    void resultReady(const QString &s);
};

#endif // NETWORKTHREAD_H
