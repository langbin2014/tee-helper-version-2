#ifndef SEACHSERVERPORTHELPER_H
#define SEACHSERVERPORTHELPER_H

#include<QUdpSocket>
#include<vector>
#include<memory>
#include<QSettings>
#include"c++/log.h"
#include<QTimer>
class SeachServerPortHelper : public QObject
{
    const QString TAG = "搜索websocket助手";
    const bool DEBUG = false;
    Q_OBJECT
    Q_PROPERTY(QString default_device_id     READ getDefault_device_id   WRITE setDefault_device_id    NOTIFY default_device_idChanged);
public:
    SeachServerPortHelper();
    ~SeachServerPortHelper();
    void init();
    static SeachServerPortHelper * get_instance();

    QString getDefault_device_id() const;
    void setDefault_device_id(const QString &default_device_id);

signals:
    void websocket_server_info(const QString & device_id,const QString & device_name,const QString & ip, const int port); //收到安卓websocket的服务器信息
    void default_device_idChanged();
//    void show_ui();//twain驱动部分
//    void hide_ui();
private:
    static int get_ui_udp_port();
  //  QTimer * timer;
    QUdpSocket* _socket;
   // qint64 _heart_pts;
   // quint16 cliet_ports[10] = {11109,11119,11129};//,11139,11149,11159,11169,11179,11189,11199};
  //  quint16 server_ports[10] = {12109,12119,12129,12139,12149,12159,12169,12179,12189,12199};
    static  SeachServerPortHelper * _self;
    QString _default_device_id = "00000000";
    const QString _websocket_device_id_key = "device_id";
   // QSettings settings;
    int _self_port/*, _dirver_port*/;
};

#endif // SEACHSERVERPORTHELPER_H
