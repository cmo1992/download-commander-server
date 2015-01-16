#include "itemlist.h"
#include "item.h"

#include <QList>
#include <QMutableListIterator>
#include <QListIterator>
#include <QDebug>
#include <QFile>
#include <QProcess>
#include <QRegExp>

//=============================================================================
// Method:   ItemList(Config*, QObject*)
// Function: Constructor - connects the signals and slots on the Config object
//           up with signals and slots of the item list object.
// Param 1:  Config *cf - Pointer to the servers config object
// Param 2:  QObject* parent - The parent objet.
// Return:   Void
//=============================================================================
ItemList::ItemList(Config *cf, QObject* parent) : QObject(parent)
{
    list = new QList<Item>();
    conf = cf;
    connect(conf, SIGNAL(send_Request_S_XML(QString*)), this, SLOT(receiveXML(QString*)));
    connect(conf, SIGNAL(send_Request_S_MD(QString*)), this, SLOT(receiveMD(QString*)));
    connect(conf, SIGNAL(send_Request_S_HTML(QString*)), this, SLOT(receiveHTML(QString*)));
    connect(conf, SIGNAL(send_Request_T_MD(QString*)), this, SLOT(receiveMD(QString*)));
    connect(conf, SIGNAL(send_Request_T_HTML(QString*)), this, SLOT(receiveHTML(QString*)));
    connect(conf, SIGNAL(send_Request_FSD(QString*)), this, SLOT(receivePath(QString*)));
    connect(conf, SIGNAL(send_Request_ARL(QString*)), this, SLOT(receiveARL(QString*)));
    connect(this, SIGNAL(requestARL()), conf, SLOT(send_ARL()));
    connect(this, SIGNAL(requestPath()), conf, SLOT(sendFSD()));
    connect(this, SIGNAL(requestXML()), conf, SLOT(sendS_XML()));
    connect(this, SIGNAL(requestMD()), conf, SLOT(sendS_MD()));
    connect(this, SIGNAL(requestHTML()), conf, SLOT(sendS_HTML()));
    sendRequestPath();
    sendRequestXML();
    sendRequestARL();
    open();
}

//=============================================================================
// Method:   ~ItemList()
// Function: Deconstructor
// Param:    None
// Return:   Void
//=============================================================================
ItemList::~ItemList() {}


///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS...
///////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Method:   addItem(QString fName, QString link)
// Function: Adds the file and link to the list of file that were uploaded. If
//           the file is already in the list it does not add the item to the
//           list and returns false.
// Param 1:  QString fName - file name
// Param 2:  QString link - the link title
// Return:   bool - if added remove true else false
//=============================================================================
bool ItemList::addItem(QString fName, QString link){
    qint64 found = find(fName);
    if(found == -1){
        Item i(fName, link);
        list->push_back(i);
        return true;
    } else {
        return false;
    }
}

//=============================================================================
// Method:   remove(QString)
// Function: Removes the item from the list.
// Param:    QString fName - file name
// Return:   bool - if removed return true else false
//=============================================================================
bool ItemList::remove(QString fName){
    bool rem = false;
    QMutableListIterator<Item> i(*list);
    while(i.hasNext()){
        Item item = i.next();
        if(item.getFile() == fName) {
            i.remove();
            rem = true;
        }
    }
    return rem;
}

//=============================================================================
// Method:   getList()
// Function: pulls the file names from all the files in the list and places
//           them in a QList<QString>.
// Param:    None
// Return:   QList<QString> fString - the list of file names.
//=============================================================================
QList<QString> ItemList::getList(){
    QList<QString> fString;
    QMutableListIterator<Item> i(*list);
    while(i.hasNext()){
        Item item = i.next();
        fString.append(item.getFile());
    }
    return fString;
}

//=============================================================================
// Method:   open()
// Function: Loads the XML file into item objects in a QList<Item>.
// Param:    None
// Return:   Viod
//=============================================================================
void ItemList::open(){
    qDebug() << "Loading File: " << *xml;
    QFile in(*xml);
    if (!in.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Failed to open file";
    } else {
        QString headerLine = in.readLine();
        QString root = in.readLine();
        QString item = in.readLine();
        QRegExp rx("(><|<|>)");
        QStringList elements = item.split(rx);
        while(elements[1] == "item"){

            Item itm(elements[3], elements[6]);
            qDebug() << "ADDED: " << itm.getFile();
            list->append(itm);

            item = in.readLine();
            elements = item.split(rx);
        }
    }
    in.close();
}
//NOT IMPLEMENTED
//=============================================================================
// Method:   sync()
// Function: Syncs the download folder and the objects in the list.
// Param:    None
// Return:   Void
//=============================================================================
void ItemList::sync(){
    qDebug() << "Syncs the folder files with the stored data";
}

//=============================================================================
// Method:   saveSite()
// Function: Generates a markdown file and then converts it to html with
//           pandoc.
// Param:    None
// Return:   Void
//=============================================================================
void ItemList::saveSite(){
    QString mdText="#[Home](index.html) > Download Page\n\n- - -\n\n";
    QMutableListIterator<Item> i(*list);
    /////////////////////////////////////////////
    QString a_path;
    if(*arl == "NO_APACHE"){
        a_path = *path;
    } else {
        a_path = *arl;
        qDebug() << "a_path: " << a_path;
    }

//////////////////////////////////////////////////
    while(i.hasNext()){
        Item item = i.next();
        mdText.append("- [" + item.getLink() + "](" + a_path + item.getFile() +")\n");
    }
    mdText.append("\n- - -\n<footer>CMO's Technical Solutions </footer>");
    qDebug() << "Writing: " << *md;
    write(md, mdText);

    qDebug() << "Writing: " << *html;
    QProcess run;
    run.execute("pandoc "+ *md +" -f markdown -t html -s -o " + *html);
    run.waitForFinished();
    qDebug() << "Site is updated!";
}

//=============================================================================
// Method:   save()
// Function: Generate and saves an xml file of all the files and links that
//           were added to the list in order to load the list when the server
//           starts up.
// Param:    None
// Return:   Void
//=============================================================================
void ItemList::save(){
    QString xmlText="<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<items>\n";
    QMutableListIterator<Item> i(*list);
    while(i.hasNext()){
        Item item = i.next();
        xmlText.append(item.toXml() + '\n');
    }
    xmlText.append("</items>\n");
    qDebug() << xmlText;
    write(xml, xmlText);
}

//=============================================================================
// Method:   sendRequestXML()
// Function: Emits the request for the XML file.
// Param:    None
// Return:   Void
//=============================================================================
void ItemList::sendRequestPath(){
    emit requestPath();
}

//=============================================================================
// Method:   sendRequestXML()
// Function: Emits the request for the XML file.
// Param:    None
// Return:   Void
//=============================================================================
void ItemList::sendRequestXML(){
    emit requestXML();
}

//=============================================================================
// Method:   sendRequestMD()
// Function: Emits the request for the MD file.
// Param:    None
// Return:   Void
//=============================================================================
void ItemList::sendRequestMD(){
    emit requestMD();
}

//=============================================================================
// Method:   sendRequestHTML()
// Function: Emits the request for the HTML file.
// Param:    None
// Return:   Void
//=============================================================================
void ItemList::sendRequestHTML(){
    emit requestHTML();
}

//=============================================================================
// Method:   sendRequestARL()
// Function: Emits the request for the Apache Root Location file.
// Param:    None
// Return:   Void
//=============================================================================
void ItemList::sendRequestARL(){
    emit requestARL();
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC SLOTS...
///////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Method:   receivePath(QString)
// Function: Receives the file storage location and stores the location.
// Param:    QString *str - The configuration string
// Return:   Void
//=============================================================================
void ItemList::receivePath(QString *str){
    path = str;
}

//=============================================================================
// Method:   receiveXML(QString)
// Function: Receives the XML file location and stores the location.
// Param:    QString *str - The configuration string
// Return:   Void
//=============================================================================
void ItemList::receiveXML(QString *str){
    xml = str;
//    qDebug() << "XML: " << *str;
}

//=============================================================================
// Method:   receiveMD(QString)
// Function: Receives the MD file location and stores the location.
// Param:    QString *str - The configuration string
// Return:   Void
//=============================================================================
void ItemList::receiveMD(QString *str){
    md = str;
//    qDebug() << "MD: " << *str;
}

//=============================================================================
// Method:   receiveHTML(QString)
// Function: Receives the HTML file location and stores the location.
// Param:    QString *str - The configuration string
// Return:   Void
//=============================================================================
void ItemList::receiveHTML(QString *str){
    html = str;
//    qDebug() << "HTML: " << *str;
}

//=============================================================================
// Method:   receiveARL(QString)
// Function: Receives the ARL location.
// Param:    QString *str - The configuration string
// Return:   Void
//=============================================================================
void ItemList::receiveARL(QString *str){
    arl = str;
//    qDebug() << "HTML: " << *str;
}

///////////////////////////////////////////////////////////////////////////////
// MAIN PRIVATE METHODS...
///////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Method:   write(QString, QString)
// Function: Writes the file with the QString.
// Param:    None
// Return:   Void
//=============================================================================
void ItemList::write(QString *path, QString text){
    qDebug() << *path;
    QFile f(*path);
    if (!f.open(QIODevice::WriteOnly)){
        qDebug() << "The file is in read only mode";
    } else {
        f.write(text.toStdString().data());
    }
}

//=============================================================================
// Method:   find(QString)
// Function: searches the list for a matching file name.
// Param:    QString fName - File name
// Return:   qint64 iIndex - index of the file if found else -1.
//=============================================================================
qint64 ItemList::find(QString fName)
{
    QListIterator<Item> i(*list);
    int iIndex = -1;
    int iTmp = 0;
    while(i.hasNext()){
        Item item = i.next();
        if(item.getFile() == fName){
            iIndex = iTmp;
            break;
        }
        iTmp++;
    }
    return iIndex;
}

///////////////////////////////////////////////////////////////////////////////
// SUB PRIVATE METHODS...
///////////////////////////////////////////////////////////////////////////////
