#ifndef MYSERVERTHREAD_H
#define MYSERVERTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QDebug>
#include "itemlist.h"



class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(int ID, QString p, QObject* parent = 0);
    ~MyThread();
    void run();
    void getList(ItemList *iList);

signals:
    void error(QTcpSocket::SocketError socketerror);
    void startJServer();
    
public slots:
    void readyRead();
    void disconnected();

private:
    //main functions
    void add();
    void remove();
    void list();
    void sendError(QString cmd);
    
    //sub functions
    void updateSite();
    void updateXML();
    void delay( int millisecondsToWait );
    QString receiveQString();
    qint64 receiveFileSize(QDataStream *in);


    //variables
    QString path;
    QTcpSocket *socket;
    int socketDescriptor;
    ItemList *iList;
};

#endif // MYSERVERTHREAD_H
