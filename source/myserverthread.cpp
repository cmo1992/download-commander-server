#include "myserverthread.h"
#include "myserver.h"
#include "item.h"
#include "myserverthread.h"
#include <QFile>
#include <QListIterator>
#include <QByteArray>
#include <QProcess>
#include <QTime>
#include <QCoreApplication>

//=============================================================================
// Method:   MyThread(int, QObject)
// Function: Constructor - The thread for the new connection.
// Param 1:  int ID - the socket descriptor
// Param 2:  QObject* parent - the parent object that created this object.
// Return:   Void
//=============================================================================
MyThread::MyThread(int ID, QString p, QObject* parent) : QThread(parent)
{
    this->socketDescriptor = ID;
    this->path = p;
}

//=============================================================================
// Method:   ~MyThread()
// Function: Deconstructor
// Param:    None
// Return:   Void
//=============================================================================
MyThread::~MyThread(){}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS...
///////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Method:   run()
// Function: The main logic to start the thread for the conection.
// Param:    None
// Return:   Void
//=============================================================================
void MyThread::run()
{
    // thread starts here
    qDebug() << socketDescriptor << " Starting thread...";

    socket = new QTcpSocket();

    // set the ID
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        // something's wrong, we just emit a signal
        emit error(socket->error());
        return;
    }

    // connect socket and signal
    // note - Qt::DirectConnection is used because it's multithreaded
    //        This makes the slot to be invoked immediately, when the
    //        signal is emitted.
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()),
            Qt::DirectConnection);

    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    // We'll have multiple clients, we want to know which is which
    qDebug() << socketDescriptor << " Client connected...";

    // make this thread a loop,
    // thread will stay alive so that signal/slot to function properly
    // not dropped out in the middle when thread dies

    exec();
}

//=============================================================================
// Method:   getList(ItemList)
// Function: Receives a pointer to the servers iList.
// Param:    ItemList *iList
// Return:   Void
//=============================================================================
void MyThread::getList(ItemList *iList){
    this->iList = iList;
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC SLOTS...
///////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Method:   readyRead()
// Function: Reads the command string to tell the server what action to take.
// Param:    None
// Return:   Void
//=============================================================================
void MyThread::readyRead(){
    
    QByteArray inData;
    inData = socket->readLine();
    QString cmd(inData);

    if (cmd == "ADD") {
        qDebug() << cmd << "to site.";
        add();
    } else if (cmd == "REM") {
        qDebug() << cmd << "file!";
        remove();
    } else if (cmd == "LST") {
        qDebug() << cmd << "will be sent.";
        list();
    } else {
        sendError(cmd);
    }
}

//=============================================================================
// Method:   disconnect()
// Function: Disconnects the socket.
// Param:    None
// Return:   Void
//=============================================================================
void MyThread::disconnected()
{
    qDebug() << socketDescriptor << " Disconnected";

    socket->deleteLater();
    exit(0);
}

///////////////////////////////////////////////////////////////////////////////
// MAIN PRIVATE METHODS...
///////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Method:   add()
// Function: Adds the file to the server. Saves a backup XML file, a
//           markdown file, and html file.
// Param:    None
// Return:   Void
//=============================================================================
void MyThread::add(){

    //receive file Name and create file obj
    QString fName = receiveQString();
    qDebug() << "Received File: " << fName;
    //receiving link
    QString linkName = receiveQString();
    qDebug() << "Received Link: " << linkName;
    qDebug() << "Adding file and link to list...";
    bool added;
    added = iList->addItem(fName,linkName);
    qDebug() << added;
    if(added){
        QString _path = path + fName;
        QFile file(_path);
        qDebug() << "Save file at: " << _path;
        //Update XML file with new file
        qDebug() << "Updating XML...";
        updateXML();
        //Update the web page
        updateSite();
        socket->write("ADD_PASS\n");
    } else {
        socket->write("ADD_FAILED\n");
    }
    socket->flush();
    socket->waitForBytesWritten();
}

//=============================================================================
// Method:   remove()
// Function: Removes the file from the download location, xml, iList, markdown
//           and html files.
// Param:    None
// Return:   Void
//=============================================================================
void MyThread::remove(){
    QString fName = receiveQString();
    qDebug() << "Removing: " << fName;
    bool tp = iList->remove(fName);
    qDebug() << tp;
    if(tp){
        qDebug() << path;
        QString rFile(path+fName);
        qDebug() << rFile;
        QString rm = "rm \"" + rFile + "\"";
        qDebug() << rm;
        QProcess runn;
        runn.execute(rm);
        runn.waitForFinished();
        qDebug() << "XML is updat...";
        //Update XML file with new file
        updateXML();
        //Update the web page
        updateSite();
        socket->write("REM_PASS\n");
    } else {
        socket->write("REM_FAILED\n");
    }
    socket->flush();
    socket->waitForBytesWritten();
}

//=============================================================================
// Method:   list()
// Function: Send a list of all the files in the system to the client.
// Param:    None
// Return:   Void
//=============================================================================
void MyThread::list(){
    QStringList ls = iList->getList();
    QString num = QString::number(ls.length());
    qDebug() << num;
    QString tmp = num + '\n';
    socket->write(tmp.toStdString().data());
    socket->flush();
    socket->waitForBytesWritten();
    QListIterator<QString> i(ls);
    while(i.hasNext()){
        QString str = i.next() + '\n';
        qDebug() << str;
        socket->write(str.toStdString().data());
        socket->flush();
        socket->waitForBytesWritten();
    }
}

//=============================================================================
// Method:   sendError(QString)
// Function: out puts an invalid command message to the console
// Param:    QString cmd - the invalid command.
// Return:   Void
//=============================================================================
void MyThread::sendError(QString cmd){
    qDebug() << "Invalid server command - " << cmd;
}

///////////////////////////////////////////////////////////////////////////////
// SUB PRIVATE METHODS...
///////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Method:   updateSite()
// Function: Updates the website.
// Param:    None
// Return:   Void
//=============================================================================
void MyThread::updateSite(){
    qDebug() << "Updating web site...";
    iList->saveSite();
}

//=============================================================================
// Method:   updateXML()
// Function: Updates the XML backup file.
// Param:    None
// Return:   Void
//=============================================================================
void MyThread::updateXML(){
    iList->save();
}

//=============================================================================
// Method:   delay(int)
// Function: Delays the process from moving on for the specifide number of
//           milliseconds.
// Param:    int millisecondsToWait - Number of milliseconds to wait.
// Return:   Void
//=============================================================================
void MyThread::delay( int millisecondsToWait ){
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

//=============================================================================
// Method:   receiveQString()
// Function: Reads a line from the socket.
// Param:    None
// Return:   QString str - the string read from the socket.
//=============================================================================
QString MyThread::receiveQString(){
    QByteArray inData;
    qDebug() << "Reading socket.............";
    delay(100);
    inData = socket->readLine();
    QString str(inData.left(inData.length()-1));
    return str;
}
