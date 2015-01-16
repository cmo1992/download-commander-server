#include <QCoreApplication>
#include "myserver.h"

//=============================================================================
// Method:   main(int, char*[])
// Function: The main logic to start the download-commander-server.
// Param 1:  int argc -
// Param 2:  char *argv[] -
// Return:   int a.exec() - exit code
//=============================================================================
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Make a server and starts it
        MyServer server;
        server.startServer();

    return a.exec();
}
