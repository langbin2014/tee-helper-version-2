#include "seachserverporthelper.h"
#include<QJsonObject>
#include<QJsonDocument>
#include<QThread>
#include"c++/tools.h"
SeachServerPortHelper * SeachServerPortHelper::_self = nullptr;
SeachServerPortHelper::SeachServerPortHelper()
{
    QTimer::singleShot(15000,this,[this]{
       init();
    });

}

SeachServerPortHelper::~SeachServerPortHelper()
{
}

void SeachServerPortHelper::init()
{
    qint64 start_pts = Tools::get_1970_ms();
    _self_port = get_ui_udp_port();
    QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat);
    _default_device_id = settings.value(_websocket_device_id_key,QVariant("00000000")).toString();
    connect(this,&SeachServerPortHelper::default_device_idChanged,this,[this]{
        QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat);
        settings.setValue(_websocket_device_id_key,_default_device_id);
    });

   qint64 now = Tools::get_1970_ms();
    qDebug() << now - start_pts<< "ms" << "4444------------";
    start_pts = now;
    _socket = nullptr;
    {
        QUdpSocket * sk = new QUdpSocket(this);
        if(sk->bind(QHostAddress("0.0.0.0"),_self_port))
        {
            Log::i(TAG, QString("udp正在监听:%1").arg( _self_port));
            _socket = sk;
            //_heart_pts = Tools::get_1970_ms();
            connect(_socket,&QUdpSocket::readyRead,this,[this]{
                while (_socket->hasPendingDatagrams()) {
                    QByteArray datagram;
                    datagram.resize(_socket->pendingDatagramSize());
                    quint16 temp_port;
                    int len = _socket->readDatagram(datagram.data(),datagram.size(),nullptr,&temp_port);
                    if(len <= 0)
                        continue;
                    {//安卓pad会不停地发udp广播，告诉它的websocket的ip和端口
                        QJsonParseError err;
                        QByteArray info =QByteArray::fromBase64(datagram);
                        QJsonDocument doc = QJsonDocument::fromJson(info, &err);
                        if(DEBUG)
                            Log::d(TAG,"收到安卓信息:"+QString(info));
                        if (err.error == QJsonParseError::NoError)
                        {
                            if(doc.isObject())
                            {
                                QJsonObject json = doc.object();
                                if(!json.empty())
                                {
                                    if( json["task"].toString() == "got_websocket_server_info")
                                    {
                                        int websocket_port = json["websocket_port"].toInt();
                                        QString websocket_ip = json["websocket_ip"].toString();
                                        QString id = json["device_id"].toString();
                                        QString name = json["device_name"].toString();
                                        emit websocket_server_info(id,name,websocket_ip,websocket_port);
                                    }
                                }
                            }
                        }
                        else
                        {
                            Log::e(TAG,"安卓的udp协议错误"+QString(datagram.toHex()));;
                        }
                    }
                }
            });
        }
        else
            sk->deleteLater();
    }
}

SeachServerPortHelper *SeachServerPortHelper::get_instance()
{
    if(_self == nullptr)
    {
        _self = new SeachServerPortHelper(); //单例，构造的时候就绑定三个端口之一了
    }
    return _self;
}

QString SeachServerPortHelper::getDefault_device_id() const
{
    return _default_device_id;
}

void SeachServerPortHelper::setDefault_device_id(const QString &default_device_id)
{
    if(_default_device_id != default_device_id)
    {
        _default_device_id = default_device_id;
        emit default_device_idChanged();
    }
}

int SeachServerPortHelper::get_ui_udp_port()
{
#ifdef UNIT_TEST
    return  11109;
#else
    quint16 cliet_ports[3] = {11109,11119,11129};
    QString local = "127.0.0.1";
    QHostAddress ip  = QHostAddress(local);
    for(int i = 0; i <3;i++)
    {
        bool ok;
        {
            QUdpSocket  sk ;// = new QUdpSocket;
            ok = sk.bind(QHostAddress::AnyIPv4,cliet_ports[i]);//,QUdpSocket::ShareAddress);
            if(ok)
            {
                sk.abort();//解除绑定
                sk.close();
            }
        }
        QThread::msleep(500);
        if(ok)
            return cliet_ports[i];
    }
    return -1;
#endif
}

