#include "config.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QCoreApplication>

//=============================================================================
// Method:   Config(QObject*)
// Function: Constructor
// Param:    QObject* parent
// Return:   Void
//=============================================================================
Config::Config(QObject* parent) : QObject(parent)
{
    f = "./download-commander-server.conf";
}

//=============================================================================
// Method:   ~Config()
// Function: Deconstructor
// Param:    None
// Return:   Void
//=============================================================================
Config::~Config() {}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS...
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// PUBLIC SLOTS...
///////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Method:   load()
// Function: Loads the server configuration file and.
// Param:    None
// Return:   Void
//=============================================================================
void Config::load(){
    QFile *cFile = new QFile(f, this);

    if(!cFile->exists()){
        qDebug() << "Creating default config file...";
        writeDefaultConfigFile(cFile);
    }
    if (!cFile->open(QIODevice::ReadOnly)){
        qDebug() << "The file is in write only mode";
    } else {
        qDebug() << "Processing config file...";
        QFileInfo *info = new QFileInfo(*cFile);
        qDebug() << info->absoluteFilePath();
        readConfigFile(cFile);
    }
    cFile->close();
}

//=============================================================================
// Method:   sendFSD()
// Function: Emits  the "send_Request_FSD" signal. Transmits the file storage
//           directory.
// Param:    None
// Return:   Void
//=============================================================================
void Config::sendFSD(){
    emit send_Request_FSD(&FILE_STORAGE_DIR);
}

//=============================================================================
// Method:   sendS_XML()
// Function: Emits  the "send_Request_S_XML" signal. Transmits the xml file
//           location.
// Param:    None
// Return:   Void
//=============================================================================
void Config::sendS_XML(){
    emit send_Request_S_XML(&SAVE_XML_LOCATION);
}

//=============================================================================
// Method:   sendS_MD
// Function: Emits  the "send_Request_S_MD" or "send_Request_T_MD" signal,
//           depending on where to store the markdown file.
// Param:    None
// Return:   Void
//=============================================================================
void Config::sendS_MD(){
    if(SAVE_MARKDOWN_FILE){
        emit send_Request_S_MD(&SAVE_MARKDOWN_LOCATION);
    } else {
        emit send_Request_T_MD(&TEMP_MARKDOWN_LOCATION);
    }
}

//=============================================================================
// Method:   sendS_HTML()
// Function: Emits  the "send_Request_S_HTML" or "send_Request_T_HTML" signal,
//           depending on where to store the html file. Either in a tmp
//           location or a production location.
// Param:    None
// Return:   Void
//=============================================================================
void Config::sendS_HTML(){
    if(SAVE_HTML_FILE){
        emit send_Request_S_HTML(&SAVE_HTML_LOCATION);
    } else {\
        emit send_Request_T_HTML(&TEMP_HTML_LOCATION);
    }
}

//=============================================================================
// Method:   send_ARL()
// Function: Emits  the "send_Request_ARL" signal.
// Param:    None
// Return:   Void
//=============================================================================
void Config::send_ARL(){
    emit send_Request_ARL(&APACHE_ROOT_LOCATION);
}

///////////////////////////////////////////////////////////////////////////////
// PROTECTED...
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// MAIN PRIVATE METHODS...
///////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Method:   writeDefaultConfigFile(QFile)
// Function: Writes the default config file if there is no config file in the
//           application's folder.
// Param:    QFile *cFile - The config file.
// Return:   Void
//=============================================================================
void Config::writeDefaultConfigFile(QFile *cFile)
{
    if (!cFile->open(QIODevice::WriteOnly)){
        qDebug() << "The file is in Read only mode";
    } else {
        cFile->write("################################################################################\n");
        cFile->write("# download-commander-server config file                                        #\n");
        cFile->write("# The properties in this file can be altered to meet your needs.               #\n");
        cFile->write("# If there are multiple of the same property the last property will be kept.   #\n");
        cFile->write("#                                                                              #\n");
        cFile->write("#                                                                              #\n");
        cFile->write("#                                                                              #\n");
        cFile->write("#                   DO NOT CHANGE THE ORDER OF THIS FILE                       #\n");
        cFile->write("#                                                                              #\n");
        cFile->write("#                                                                              #\n");
        cFile->write("# This is a comment in this config file                                        #\n");
        cFile->write("#                                                                              #\n");
        cFile->write("################################################################################\n");
        cFile->write("#\n");
        cFile->write("# This is the directory the files will be saved to.\n");
        cFile->write("# Linux\n");
        cFile->write("FILE_STORAGE_DIR: /var/www/html/downloads/\n");
        cFile->write("#FILE_STORAGE_DIR: /tmp/t/\n");
        cFile->write("#\n");
        cFile->write("# Apache Settings:\n");
        cFile->write("STORAGE_FROM_APACHE_ROOT_LOCATION: /downloads/\n");
        cFile->write("#STORAGE_FROM_APACHE_ROOT_LOCATION: NO_APACHE\n");
        cFile->write("#\n");
        cFile->write("# XML Settings:\n");
        cFile->write("#Note: The XML file is a back up for if the server carches or closes all the XML\n");
        cFile->write("#      file will load all the saved file details and links to load back into the\n");
        cFile->write("#      server when restarted.\n");
        cFile->write("SAVE_XML_LOCATION: /var/www/html/downloads.xml\n");
        cFile->write("#SAVE_XML_LOCATION: /tmp/t/downloads.xml\n");
        cFile->write("#\n");
        cFile->write("# Markdown Settings:\n");
        cFile->write("SAVE_MARKDOWN_FILE: YES\n");
        cFile->write("#SAVE_MARKDOWN_FILE: NO\n");
        cFile->write("SAVE_MARKDOWN_LOCATION: /var/www/html/downloads.md\n");
        cFile->write("TEMP_MARKDOWN_LOCATION: /tmp/downloads.md\n");
        cFile->write("#\n");
        cFile->write("# HTML Settings:\n");
        cFile->write("SAVE_HTML_FILE: YES\n");
        cFile->write("#SAVE_HTML_FILE: NO\n");
        cFile->write("SAVE_HTML_LOCATION: /var/www/html/downloads.html\n");
        cFile->write("TEMP_HTML_LOCATION: /tmp/downloads.html");
    }
    cFile->close();
}

//=============================================================================
// Method:   readConfigFile(QFile)
// Function: Reads each line of the file and checks if it is commented out or
//           not. If commented ('#' at begining of line) then reads in next
//           line and checks if again. If not commented it splits the line into
//           tokens and passes then to "configLogic(QStringList)."
// Param:    None
// Return:   Void
//=============================================================================
void Config::readConfigFile(QFile *cFile)
{
    while(!cFile->atEnd()){
        QString line;
        line = cFile->readLine();
        if(line.at(0) != '#'){
            QStringList tokens = line.split(": ");
            configLogis(tokens);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// SUB PRIVATE METHODS...
///////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Method:   configLogis(QStringList)
// Function: Take the tokens and matches them to the correct veriables. If
//           invalid configuration is read in, the server notifies the admin
//           and then offers to write a default config file next to the config
//           file for comparison.
// Param:    QStringList tokens - The Tokens from the config file.
// Return:   Void
//=============================================================================
void Config::configLogis(QStringList tokens)
{
    QString tp = tokens[1].mid(0, tokens[1].length() - 1);
    if(tokens[0] == "FILE_STORAGE_DIR"){
        FILE_STORAGE_DIR = tp;
    } else if(tokens[0] == "SAVE_XML_LOCATION"){
        SAVE_XML_LOCATION =tp;
    } else if(tokens[0] == "SAVE_MARKDOWN_FILE"){
        if(tp == "YES"){
            SAVE_MARKDOWN_FILE = true;
        } else {
            SAVE_MARKDOWN_FILE = false;
        }
    } else if(tokens[0] == "SAVE_MARKDOWN_LOCATION"){
        SAVE_MARKDOWN_LOCATION =tp;
    } else if(tokens[0] == "TEMP_MARKDOWN_LOCATION"){
        TEMP_MARKDOWN_LOCATION =tp;
    } else if(tokens[0] == "SAVE_HTML_FILE"){
        if(tp == "YES"){
            SAVE_HTML_FILE = true;
        } else {
            SAVE_HTML_FILE = false;
        }
    } else if(tokens[0] == "SAVE_HTML_LOCATION"){
        SAVE_HTML_LOCATION =tp;
    } else if(tokens[0] == "TEMP_HTML_LOCATION"){
        TEMP_HTML_LOCATION =tp;
    } else if(tokens[0] == "APACHE_IS_INSTALLED"){
        if(tp == "YES"){
            APACHE_IS_INSTALLED = true;
        } else {
            APACHE_IS_INSTALLED = false;
        }
    } else if(tokens[0] == "STORAGE_FROM_APACHE_ROOT_LOCATION"){
        APACHE_ROOT_LOCATION =tp;
    } else {
        qWarning() << "ERROR: Not a valid configureation!!!";
        qWarning() << "INPUT: " << tokens.join(": ");
        qWarning() << "Please make correction in config file.";
        qWarning() << "Would you like a default config file to";
        qWarning() << "be written to compare with the error?";
        qWarning() << "{Will NOT overwrite current config} (yes or no): ";
        QTextStream s(stdin);
        QString value = s.readLine();
        if ((value.toUpper())[0] == 'Y'){
            QFile tmp(f + ".default");
            writeDefaultConfigFile(&tmp);
        }
        qWarning() << "Server is shutting down...";
        emit shutdownServer();
    }
    qDebug() << tokens[0] << ", " << tp;
}
