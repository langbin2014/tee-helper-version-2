


#include"c++/configure.h"
#include "data_receive.h"
#include <QDebug>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QMetaEnum>
#include <QFile>
#include <QDataStream>
#include <QDir>
#include <QImage>
#include "c++/log.h"
#include "c++/tools.h"
#include <core/image_core.h>
//static QString url = "ws://192.168.5.20:8090/socketHandler?deviceNum=4&mac=AA-BB-CC-DD-EE-FF&version=2.0";
//此函数导致程序崩溃,ip和port可能不存在


#include "data_receive.h"
#include <QDebug>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QMetaEnum>
#include <QFile>
#include <QDataStream>
#include <QDir>
#include <QImage>
#include"c++/image_factor.h"

#include <core/image_core.h>
//static QString url = "ws://192.168.5.20:8090/socketHandler?deviceNum=4&mac=AA-BB-CC-DD-EE-FF&version=2.0";
//此函数导致程序崩溃,ip和port可能不存在
#include "c++/managers/patient_manager.h"
#include"c++/table/images_table.h"
#include<QThread>

void DataReceive::set_on_new_picture_listener(std::function<void (const QImage * pic)> on_new_picture)
{
    this->on_new_picture = on_new_picture;
}

DataReceive::DataReceive(QObject *parent):QObject(parent)
{
    dataRecvWS = new QWebSocket(QString(),QWebSocketProtocol::VersionLatest,this);
   _conn = connect(dataRecvWS,&QWebSocket::disconnected,this,&DataReceive::onDisConnected,Qt::AutoConnection);
    connect(dataRecvWS,&QWebSocket::textMessageReceived,this,&DataReceive::onTextReceived,Qt::AutoConnection);
    connect(dataRecvWS,&QWebSocket::connected,this,&DataReceive::onConnected,Qt::AutoConnection);
    dataRecvWS->ignoreSslErrors();
    _connected = false;
    _status = Status::Stoped;
    m_timer = new QTimer(this);
    connect(m_timer,&QTimer::timeout,[this]{
        if( _connected)
        {
            dataRecvWS->sendTextMessage("1");
        }
    });
    m_timer->start(4000);
}

DataReceive::~DataReceive(){

}

void DataReceive::stop_listen()
{
    if(_status != Status::Stoped)
    {
        _status = Status::Stoped;
        dataRecvWS->close();
        QThread::msleep(10);
        dataRecvWS->abort();
        url = "";
    }
    _old_ip = "";
    _old_port = -1;
}

bool DataReceive::getSerial_connected() const
{
    return _serial_connected;
}

void DataReceive::setSerial_connected(bool serial_connected)
{
    if(_serial_connected != serial_connected)
    {
       _serial_connected = serial_connected;
       emit serial_connectedChanged();
    }
}

bool DataReceive::getConnected() const
{
    return _connected;
}

void DataReceive::setConnected(bool connected)
{
    if(_connected != connected)
    {
        _connected = connected;
        emit connectedChanged();
    }
}



bool DataReceive::start_listen(const QString & ip, int port){
    if(ip == "" || port <= 0)
        return false;
    if(_old_ip == ip && _old_port == port)
        return false;
    _status = Status::Starting;
    _old_ip = ip ;
    _old_port = port;
    setConnected(false);

    if(_status != Status::Stoped)
    {
        dataRecvWS->close();
        dataRecvWS->abort();
    }
    QThread::msleep(10);
    this->url = QString("ws://%1:%2/scanner/mac?").arg(ip).arg(port) +Tools::get_mac_address();
    Log::i(TAG,"websocket完整连接 = " + url);
    dataRecvWS->open(QUrl(url));
    return true;
}


void DataReceive::onConnected(){
    setConnected(true);
    _status = Status::Connected;
    Log::i(TAG,"websocket已经连接上: =" +dataRecvWS->peerAddress().toString()+":" +QString::number(dataRecvWS->peerPort()));
}


void DataReceive::onDisConnected(){//只有自动断开时会触发词函数,设置设备号不会进来这里
    Log::d(TAG,"websocket 已经断开了链接");
    setConnected(false);
    if(_status != Status::Stoped) //重连
    {
        disconnect(_conn);
        _status = Status::Disconnected;
        QTimer::singleShot(3000,this,[this]{
            if(_status != Status::Stoped )
            {
                dataRecvWS->close();
                dataRecvWS->abort();
            }
            QThread::msleep(10);
            _conn = connect(dataRecvWS,&QWebSocket::disconnected,this,&DataReceive::onDisConnected,Qt::AutoConnection);
            dataRecvWS->open(QUrl(url));

        });
    }
    else
    {//手动断开后，就不再重连

    }
}


void DataReceive::onTextReceived(const QString & msg){
    if(msg.length() == 1)//心跳
        return ;
    if(msg.length() < 100)
        Log::i(TAG,"收到websocket信息:" + msg);
    else
        Log::i(TAG,"收到websocket信息:" + msg.mid(0,100));
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8(), &err);
    if (err.error == QJsonParseError::NoError)
    {
        if (doc.isObject())
        {
            QJsonObject obj = doc.object();
            QString messgae = obj["message"].toString();
            if(messgae == "new_picture")
            {
                QString picture_data = obj["picture_data"].toString();
                int w = obj["picture_width"].toInt();
                int h = obj["picture_height"].toInt();
  //              int per_pixel_bytes = obj["picture_pixel_per_byte"].toInt();
                QByteArray buf = QByteArray::fromBase64(picture_data.toLatin1());
                qint64 now = Tools::get_1970_ms();
//                QString all_pictures_dir = Tools::getPicturesDir();
//                QDateTime current_date_time = QDateTime::currentDateTime();
//                QString current_date = current_date_time.toString("yyyy_MM_dd+hh_mm_ss");
//                QStringList dir_and_file_name = current_date.split("+");
//                QString dir =  dir_and_file_name[0];
//                QString name = dir_and_file_name[1];
//                QString path = all_pictures_dir+"/"+dir;
//                QDir dir_checker;
//                if(dir_checker.exists(path) == false)
//                    dir_checker.mkdir(path);


                const QString in_database_path = Tools::create_file_name(now,Configure::Origin_Image_Store_Node_Dir_Name,"png");//
              //  QString in_database_path =    Tools::create_file_name(now,"Pictures","png");//   path+"/"+name+".png";
                if(DEBUG)
                {
#ifdef Q_OS_WIN32
                    QString raw_path =  Tools::create_file_name(now,"Raws","raw");
                    QFile file(Tools::get_root_path()+"/"+ raw_path); //扫描仪数据
                    file.open(QIODevice::WriteOnly);
                    file.write(buf);
#endif
                }
                QString origin_path;
#ifdef Q_OS_WIN32
               origin_path  = Tools::get_root_path()+"/"+  Tools::create_file_name(now,"Origins","png");//未裁剪图片
#else
                  origin_path = "";
#endif

                QImage * pic = Core::ImageTool::ct_vaule_to_image(w,h,buf,DEBUG ? origin_path : "",false);
                if(DEBUG)
                {
#ifdef Q_OS_WIN32
                  QString clip_path  = Tools::get_root_path()+"/"+  Tools::create_file_name(now,"Clipeds","png");//裁剪后图片
                  Core::ImageTool::save(clip_path,*pic);
#endif
                }

                bool ok = Core::ImageTool::save(Tools::get_root_path()+"/"+ in_database_path,*pic);
                if(ok)
                {
                   // emit newPicture(full_path);
                   const  QString last_state_path  =  Tools::create_file_name(now,Configure::Last_State_Image_Store_Node_Dir_Name,"png");
                    {
                        QImage * final_pic = ImageFactory::use_default_value_create_an_final_image(*pic);

                         Core::ImageTool::save(Tools::get_root_path()+"/"+ last_state_path,*final_pic);
                         delete final_pic;
                    }
                   const QString pillar_path  =  Tools::create_file_name(now,Configure::Pill_Line_Image_Store_Node_Dir_Name,"png");//柱状图
                    {
                        QImage result;
                        Core::ImageTool::create_pillar_iamge(*pic,result,100,Qt::green,Qt::black);
                        result.save(Tools::get_root_path()+"/"+ pillar_path);
                    }
                    PatientManager * instance = PatientManager::get_instance();
                    PictureManager * picture_manager = instance->get_picture_manager();
                    //[ my_tr(lange_index,"Scaner"),my_tr(lange_index,"Import"),my_tr(lange_index,"Sensor")] //扫描仪,导入,传感器
                    QJsonObject new_pic =  picture_manager->on_new_image(in_database_path,last_state_path,pillar_path,Configure::Window_Begin,Configure::Window_End,now,ImagesTable::PictureSoureType::Scaner,pic->width(),pic->height(),41.0f/500.0f,31.0f/340.0f);
                    //picture_manager->setCollecting_lastest_picture_url(in_database_path);
                    picture_manager->set_last_pic_info(new_pic);
                    picture_manager->setLastest_picture_id(QString::number(now));
                    emit got_image();
                }
                else
                    Log::e(TAG,"保存文件失败:"+in_database_path);
                 delete  pic;


//                if(on_new_picture != nullptr)
//                {
//                    QImage * pic = on_new_picture(w,h,2,buf);
//                    if(pic != nullptr)
//                    {
//                        quint64 ptr = (quint64)pic;
//                        emit newPicture(QString::number(ptr,16));
//                    }
//                }

            }
            else if(messgae == "status_changed")
            {
                //"status": 0x01,//(变量)
                // 状态码
                        //0x01=正在扫描
                       //0x02=扫描完成
                       // 0x04=正在擦除牙片
                       //0x08=请取出
                        //上面是串口协议部分，
                        /*0x10 =正在弹出中，//这个状态只能自己通过代码判断，通过发
                送扫描命令，如果1s内部没有触发<Scanning =0x01,//=正	在扫描>就是正在弹出*/
              emit scanner_action_changed(obj["status"].toInt()); //通知界面做同步
            }
            else if(messgae == "serial_connected_status_changed")
            {
                setSerial_connected(obj["connected"].toBool());
            }
            else
            {

            }
        }
        else
            Log::e(TAG, "websocket无法把str转 json--11111");
    }
    else
        Log::e(TAG, "websocket无法把str转 json--111111");
}

