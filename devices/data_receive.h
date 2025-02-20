
#ifndef DATARECEIVE_H
#define DATARECEIVE_H

#include <QObject>
#include <QWebSocket>
#include <QString>
#include <QTimer>
#include<QJsonObject>
#include"c++/log.h"
#include<QThread>
#include<QMutex>

class DataReceive : public QObject
{
    const bool DEBUG = true;
    const QString TAG = "websocket 类 ";
    QString url ;
    std::function<void (const QImage * pic)> on_new_picture = nullptr;
     Q_PROPERTY(bool connected     READ getConnected   WRITE setConnected     NOTIFY connectedChanged);
    Q_PROPERTY(bool serial_connected     READ getSerial_connected   WRITE setSerial_connected     NOTIFY serial_connectedChanged);
    Q_OBJECT
public:
    enum Status{
        Stoped,
        Starting,
        Connected,
        Disconnected
    };
    enum PadStatus{
        Scanning =0x01,//=正在扫描
        ScanFinish = 0x02,//=扫描完成
        ClearTeeCard = 0x04,//=正在擦除牙片
        PleaseTakeOut = 0x08,//=请取出
        //上面是串口协议部分，
        PadSerialNotConnected = 0x09, //安卓设备的串口还没有连接
        Popuping = 0x10 //正在弹出中，//这个状态只能自己通过代码判断，通过发送扫描命令，如果1s内部没有触发<Scanning =0x01,//=正在扫描>就是正在弹出


    };

    void set_on_new_picture_listener( std::function<void (const QImage * pic)> on_new_picture);
    DataReceive(QObject *parent=0);
    ~DataReceive();
    Q_INVOKABLE void add_message(const QString & msg)
    {
        if(dataRecvWS!=nullptr)
        {
            Log::i(TAG,"websocket发送了:"+msg);
            dataRecvWS->sendTextMessage(msg);
        }
    }
   Q_INVOKABLE  bool start_listen(const QString &ip, int port);
    bool getConnected() const;
    void setConnected(bool connected);

    bool getSerial_connected() const;
    void setSerial_connected(bool serial_connected);

public slots:
    Q_INVOKABLE void stop_listen();    /*-<创建websocket连接 */
private:
    QWebSocket *dataRecvWS;     /*-<websocket类 */
    QMetaObject::Connection  _conn;
    bool _connected = false;         /*-<websocket连接状态，连接成功：true；断开：false */
    bool _serial_connected = false;
    Status _status = Status::Stoped;
    QTimer *m_timer;            /*-<周期重连Timer */
    int _old_port = -1;
    QString _old_ip = "12";
signals:
    void connectedChanged();
    void serial_connectedChanged();
    void scanner_action_changed(int action);//扫描仪动作
   // void newPicture(const QString & pic_ptr);
    void got_image();
private slots:
    void onConnected();                 /*-<socket建立成功后，触发该函数 */
    void onTextReceived(const QString &msg);   /*-<收到Sev端的数据时，触发该函数 */
    void onDisConnected();              /*-<socket连接断开后，触发该函数 */
};

#endif // DATARECEIVE_
