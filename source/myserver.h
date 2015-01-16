#ifndef MYSERVER_H
#define MYSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QDebug>
#include <QTcpSocket>
#include "itemlist.h"
#include "myserverthread.h"
#include "jservhandler.h"

class MyServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit MyServer(QObject* parent = 0);
    ~MyServer();
    void startServer();
    ItemList *iList;

signals:
    void loadConfig();
    void requestFileStoragePath();
    void killJserv();
    void sigStartJServer();

public slots:
    //void sendList(ItemList *iList);
    void receiveFilePath(QString *str);
    void startJServer();
    void shutdownServer();

protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    //main functions
    void delay( int millisecondsToWait );
    void startJServ();
    
    //sub functions

    
    //variables
    QString path;
    Config *conf;
    JServHandler *jServ;
    QThread *js;
};

#endif // MYSERVER_H
