#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QFile>

class Config : public QObject
{
    Q_OBJECT
public:
    explicit Config(QObject* parent = 0);
    ~Config();


signals:
    void send_Request_FSD(QString *str);
    void send_Request_S_XML(QString *str);
    void send_Request_S_MD(QString *str);
    void send_Request_T_MD(QString *str);
    void send_Request_S_HTML(QString *str);
    void send_Request_T_HTML(QString *str);
    void send_Request_ARL(QString *str);
    void shutdownServer();

public slots:
    void load();
    void sendFSD();
    void sendS_XML();
    void sendS_MD();
    void sendS_HTML();
    void send_ARL();

private:
    //main functions
    void readConfigFile(QFile *cFile);
    void writeDefaultConfigFile(QFile *cFile);

    //sub functions
    void configLogis(QStringList tokens);



    //variables
    QString f;

    QString FILE_STORAGE_DIR;
    QString SAVE_XML_LOCATION;
    QString SAVE_MARKDOWN_LOCATION;
    QString TEMP_MARKDOWN_LOCATION;
    QString SAVE_HTML_LOCATION;
    QString TEMP_HTML_LOCATION;
    QString APACHE_ROOT_LOCATION;

    bool SAVE_MARKDOWN_FILE;
    bool SAVE_HTML_FILE;
    bool APACHE_IS_INSTALLED;
};

#endif // CONFIG_H
