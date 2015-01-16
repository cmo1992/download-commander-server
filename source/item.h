#ifndef FILE_H
#define FILE_H

#include<QObject>

class Item
{
public:
    explicit Item(QString fName, QString lTitle);
    ~Item();
    QString getFile();
    void setFile(QString f);
    QString getLink();
    void setLink(QString l);
    QString toXml();

signals:

public slots:

private:
    //main functions

    //sub functions


    //variables
    QString fileName;
    QString linkTitle;
};

#endif // FILE_H
