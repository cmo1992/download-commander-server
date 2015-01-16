#ifndef JSERVHANDLER_H
#define JSERVHANDLER_H

#include <QObject>
#include <QProcess>

class JServHandler : public QObject
{
    Q_OBJECT
public:
    explicit JServHandler(QString iPath);
    ~JServHandler();
    void doSetup(QThread *cthread);

public slots:
    void startJServ();
    void killJserv();

signals:
    void logReady(QString result);

private:
    //main functions

    //sub functions


    //variables
    QString path;
    QProcess runn;

};

#endif // JSERVHANDLER_H
