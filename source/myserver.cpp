#include "myserver.h"
#include "config.h"
#include "jservhandler.h"
#include <QCoreApplication>
#include <QTime>

//=============================================================================
// Method:   MyServer(QObject*)
// Function: Constructor - Creates the download-commander-server object and
//           preforms inicial configuration and loading of ItemList.
// Param:    QObject* parent -  The parent that created this object.
// Return:   Void
//=============================================================================
MyServer::MyServer(QObject* parent) :
    QTcpServer(parent)
{
    qDebug() << "===========================================================";
    conf = new Config(this);
    connect(this,SIGNAL(loadConfig()), conf, SLOT(load()));
    connect(this,SIGNAL(requestFileStoragePath()),conf, SLOT(sendFSD()));
    connect(conf, SIGNAL(send_Request_FSD(QString*)), this, SLOT(receiveFilePath(QString*)));
    emit loadConfig();
    delay(200);
    emit requestFileStoragePath();
    qDebug() << "===========================================================";
    iList = new ItemList(conf, this);
    delay(100);
    iList->sendRequestMD();
    delay(100);
    iList->sendRequestHTML();

    qDebug() << "Server Created...";
    qDebug() << "===========================================================";
}

//=============================================================================
// Method:   ~MyServer()
// Function: Deconstructor - Kills the js thread.
// Param:    None
// Return:   Void
//=============================================================================
MyServer::~MyServer() {
    emit killJserv();
    js->exit();
    js->deleteLater();
}


///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS...
///////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Method:   startServer()
// Function: Starts the server to listen on port 1234.
// Param:    None
// Return:   Void
//=============================================================================
void MyServer::startServer(){

    if(!this->listen(QHostAddress::Any, 1234))
    {
        qDebug() << "Could not start server!";
        exit(0);
    } else {
        startJServ();
        qDebug() << "Listening to port " << 1234 << "...";

    }
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC SLOTS...
///////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Method:   receiveFilePath(QString)
// Function: Receives and stores the location of where the files are located.
// Param:    QString *str - The location from the config object.
// Return:   Void
//=============================================================================
void MyServer::receiveFilePath(QString *str){
    path = *str;
}

//=============================================================================
// Method:   startJServer()
// Function: Starts the Java plugin server.
// Param:    None
// Return:   Void
//=============================================================================
void MyServer::startJServer(){
    startJServ();
}

///////////////////////////////////////////////////////////////////////////////
// PROTECTED...
///////////////////////////////////////////////////////////////////////////////

// This function is called by QTcpServer when a new connection is available.
//=============================================================================
// Method:   incomingConnection(qintptr)
// Function: Handles every new connection and creates a new thread for it.
// Param:    qintptr socketDescriptor - the socket desctiptor
// Return:   Void
//=============================================================================
void MyServer::incomingConnection(qintptr socketDescriptor)
{
    // We have a new connection
    qDebug() << socketDescriptor << " Connecting...";

    // Every new connection will be run in a newly created thread
    qDebug() << "Creating new thread...";
    MyThread *thread = new MyThread(socketDescriptor, path, this);
    connect(thread,SIGNAL(finished()),thread, SLOT(deleteLater()));
    qDebug() << "Linking List...";
    thread->getList(iList);
    qDebug() << "Starting the thread...";

    thread->start();
}

//=============================================================================
// Method:   shutdownServer()
// Function: Closes the server.
// Param:    None
// Return:   Void
//=============================================================================
void MyServer::shutdownServer(){
    exit(0);
}

///////////////////////////////////////////////////////////////////////////////
// MAIN PRIVATE METHODS...
///////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Method:   delay(int)
// Function: Delays the process from moving on for the specifide number of
//           milliseconds.
// Param:    int millisecondsToWait - Number of milliseconds to wait.
// Return:   Void
//=============================================================================
void MyServer::delay( int millisecondsToWait ){
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

//=============================================================================
// Method:   startJServ()
// Function: Starts the Java plugin server on a new thread.
// Param:    None
// Return:   Void
//=============================================================================
void MyServer::startJServ() {
    js = new QThread(this);
    jServ = new JServHandler(path);
    jServ->doSetup(js);
    jServ->moveToThread(js);

    js->start();

}
///////////////////////////////////////////////////////////////////////////////
// SUB PRIVATE METHODS...
///////////////////////////////////////////////////////////////////////////////



