#include "jservhandler.h"
#include "myserver.h"
#include <QProcess>
#include <QThread>

//=============================================================================
// Method:   JServHandler(QString)
// Function: Constructor
// Param:    QString iPath - The path the the location where the files are
//           being written to.
// Return:   Void
//=============================================================================
JServHandler::JServHandler(QString iPath){
    path = iPath;
}

//=============================================================================
// Method:   ~JServHandler()
// Function: Deconstructor
// Param:    None
// Return:   Void
//=============================================================================
JServHandler::~JServHandler() {
    this->deleteLater();
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS...
///////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Method:   doSetup(QThread)
// Function: Connects the signals and slot up between the object and the
//           thread.
// Param:    QThread *cthread - The thread the object is being moved to.
// Return:   Void
//=============================================================================
void JServHandler::doSetup(QThread *cthread){
    connect( cthread, SIGNAL(finished()), this, SLOT(killJserv()));
    connect(cthread, SIGNAL(started()), this, SLOT(startJServ()));

}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC SLOTS...
///////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Method:   startJServ()
// Function: Starts the java plugin server and pass the location to save the
//           files.
// Param:    None
// Return:   Void
//=============================================================================
void JServHandler::startJServ(){
    qDebug() << "===========================================================";
    qDebug() << "Starting Java Transfer plugin...";
    qDebug() << "Save Location: " << path;
    runn.execute("java -jar jServ.jar \"" + path + "\"" );
    qDebug() << "Java plugin started.";
    qDebug() << "===========================================================";

    runn.waitForFinished();
}


//=============================================================================
// Method:   killJserv()
// Function: Kills the java plugin server.
// Param:    None
// Return:   Void
//=============================================================================
void JServHandler::killJserv(){
    runn.kill();
}

///////////////////////////////////////////////////////////////////////////////
// PROTECTED...
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// MAIN PRIVATE METHODS...
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// SUB PRIVATE METHODS...
///////////////////////////////////////////////////////////////////////////////
