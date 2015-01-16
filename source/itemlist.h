#ifndef ITEMLIST_H
#define ITEMLIST_H

#include "item.h"
#include "config.h"
#include <QList>
#include <QString>
#include <QObject>

class ItemList : public QObject
{
    Q_OBJECT
public:
    explicit ItemList(Config *conf, QObject *parent);
    ~ItemList();

    bool addItem(QString fName, QString link );
    bool remove(QString fName);
    QList<QString> getList();
    void save();    
    void open(); //TODO: Move to a slot
    void sync();
    void saveSite();
    void sendRequestXML();
    void sendRequestMD();
    void sendRequestHTML();
    void sendRequestPath();
    void sendRequestARL();

signals:
    void requestPath();
    void requestARL();
    void requestXML();
    void requestMD();
    void requestHTML();

public slots:
    void receivePath(QString *str);
    void receiveARL(QString *str);
    void receiveXML(QString *str);
    void receiveMD(QString *str);
    void receiveHTML(QString *str);
private:
    //main functions
    void write(QString *path, QString xml);
    qint64 find(QString fName);

    //sub functions
    
    
    //variables
    Config *conf;
    QList<Item> *list;
    QString *xml, *md, *html, *path, *arl;
};

#endif // ITEMLIST_H
