#ifndef WORKER_MANAGER_H
#define WORKER_MANAGER_H

#include<QObject>
#include<QJsonObject>
#include<QJsonArray>
#include"../tools.h"
#include<QRect>
#include"../define_tool.h"
#include"../models/doctor_model.h"

#include<QMetaEnum>
#include<QSettings>
class Data_Manager:public QObject
{
 //   DEFINE_PRO_NO_AUTO_GET_SET(QJsonArray,worker_model_json_array,QJsonArray())
//    DEFINE_PRO_NO_AUTO_GET_SET(QString,account,"")
//    DEFINE_PRO_NO_AUTO_GET_SET(QString,password,"")
    DEFINE_PRO_NO_AUTO_GET_SET(QJsonObject,current_doctor_info,QJsonObject())
    DEFINE_PRO_NO_AUTO_GET_SET(bool,logined,false)
    DEFINE_PRO_NO_AUTO_GET_SET(int,page_index,0)
    Q_OBJECT
public:
    Data_Manager( QObject *parent = nullptr);
    void load_login_data();
    Q_INVOKABLE bool login(const QString & account,const QString &password);
    Q_INVOKABLE QJsonObject get_login_page_info();
    static Data_Manager *get_instance();
     Q_INVOKABLE QObject * get_model(){
        return model;
    }

    Q_INVOKABLE QJsonObject get_info(const QString & account);
    Q_INVOKABLE void update_work(const QString & account,const QJsonObject &info);
    Q_INVOKABLE void remove(const QString & account);
    Q_INVOKABLE bool add(const QJsonObject &info);
signals:
  //  DEFINE_SIG(worker_model_json_array)
//    DEFINE_SIG(account)
//    DEFINE_SIG(password)
    void show_msg(const QString &msg);
    DEFINE_SIG(current_doctor_info)
    DEFINE_SIG(logined)
    DEFINE_SIG(page_index)
    private:
        static Data_Manager *_instance;
    Doctor_Model *model = nullptr;
};

#endif // WORKER_MANAGER_H
