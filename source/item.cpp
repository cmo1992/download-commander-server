#include "item.h"
#include <QDebug>

//=============================================================================
// Method:   Item(QString, QString)
// Function: Constructor
// Param 1:  QString fName - The file name.
// Param 2:  QString lTitle - the link that is used when generating webpage.
// Return:   Void
//=============================================================================
Item::Item(QString fName, QString lTitle)
{
    fileName = fName;
    linkTitle = lTitle;
    //qDebug() << "{" << fName << " " << lTitle << "}";
}

//=============================================================================
// Method:   ~Item()
// Function: Deconstructor
// Param:    None
// Return:   Void
//=============================================================================
Item::~Item(){}


///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS...
///////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Method:   getFile()
// Function: Returns the file name.
// Param:    None
// Return:   QString fileName - the file name.
//=============================================================================
QString Item::getFile(){
    return fileName;
}

//=============================================================================
// Method:   setFile(QString)
// Function: Sets the file name;
// Param:    QString f - the new file name.
// Return:   Void
//=============================================================================
void Item::setFile(QString f){
    fileName = f;
}

//=============================================================================
// Method:   getLink()
// Function: returns the link QString.
// Param:    None
// Return:   QString linkTitle - the link string.
//=============================================================================
QString Item::getLink(){
    return linkTitle;
}

//=============================================================================
// Method:   setLink(QString)
// Function: Sets the link QString.
// Param:    QString l - the new link.
// Return:   Void
//=============================================================================
void Item::setLink(QString l){
    linkTitle = l;
}

//=============================================================================
// Method:   toXml()
// Function: generates a xml item for the object.
// Param:    None
// Return:   QString xml - xml QSting.
//=============================================================================
QString Item::toXml(){
    QString xml = "<item><filename>" +fileName+"</filename><linktitle>"+
            linkTitle+"</linktitle></item>";
    return xml;
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC SLOTS...
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// MAIN PRIVATE METHODS...
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// SUB PRIVATE METHODS...
///////////////////////////////////////////////////////////////////////////////


