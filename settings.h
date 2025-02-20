#ifndef SETTINGS_H
#define SETTINGS_H

#include<QObject>
#include<QMetaEnum>
#include<QUrl>
#include<QSettings>
#include"tools.h"
#include<QCoreApplication>
#include"define_tool.h"


#include"langs.h"
#ifdef WIN32
static QString t1= "file:///";
#else
static QString t1 = "file://";
#endif


#include<QJsonObject>
class Settings : public QObject
{
    Q_OBJECT
public:

    enum class  Fields{
      //  data_path_dir_string,
        logined_worker_string, //成功登录了的医生
        worker_password_string, //密码
        remember_password_bool,//记住密码 自动加载密码字符串到输入框
        langage_index_int,

        clinic_name_string,//名称
        clinic_webside_string,//网址
        clinic_phone_string,//电话
        clinic_fax_string,//传真
        clinic_address_string,//地址
        clinic_post_string,//邮政编号
        clinic_country_string,//国家
        clinic_province_string,//省份
        clinic_city_string, //城市
       // clinic_font_family_string,//字体

        pacs_ip_string,
        pacs_port_int,
        pacs_ae_string,
        usb_enable_denoise_bool,
        usb_enable_fixed_bool,

        Endfield
    };

    Q_ENUM(Fields);
    static   void set_ptr( char **ptr){
       Settings::ptr = ptr;
    }

 Q_INVOKABLE   static  QString getData_path(){
        if(_data_path->isNull())
        {
            QSettings settings(QSettings::NativeFormat, QSettings::UserScope, ptr[0], ptr[1]);
            *_data_path = settings.value("data_path",QCoreApplication::applicationDirPath()+"/data").toString();
        }
        return *_data_path;
    }

  Q_INVOKABLE  static  void setData_path(const QString &path){
        if(path != _data_path)
        {
            *_data_path =  path;
            QSettings settings(QSettings::NativeFormat, QSettings::UserScope, ptr[0], ptr[1]);
            settings.setValue("data_path",*_data_path);
           // emit data_pathChanged();
        }
    }

private:

   // DEFINE_PRO(QString,data_path_dir_string,data_path_dir_string,toString,QCoreApplication::applicationDirPath()+"/data")
    DEFINE_PRO(QString,logined_worker_string,logined_worker_string,toString,"")
    DEFINE_PRO(QString,worker_password_string,worker_password_string,toString,"")
    DEFINE_PRO(bool,remember_password_bool,remember_password_bool,toBool,false)
    DEFINE_PRO(int,langage_index_int,langage_index_int,toInt,1)


    DEFINE_PRO(QString,clinic_name_string,clinic_name_string,toString,"")
    DEFINE_PRO(QString,clinic_webside_string,clinic_webside_string,toString,"")
    DEFINE_PRO(QString,clinic_phone_string,clinic_phone_string,toString,"")
    DEFINE_PRO(QString,clinic_fax_string,clinic_fax_string,toString,"")
    DEFINE_PRO(QString,clinic_address_string,clinic_address_string,toString,"")
    DEFINE_PRO(QString,clinic_post_string,clinic_post_string,toString,"")
    DEFINE_PRO(QString,clinic_country_string,clinic_country_string,toString,"")
    DEFINE_PRO(QString,clinic_province_string,clinic_province_string,toString,"")
    DEFINE_PRO(QString,clinic_city_string,clinic_city_string,toString,"")

    DEFINE_PRO(QString,pacs_ip_string,pacs_ip_string,toString,"")
    DEFINE_PRO(int,pacs_port_int,pacs_port_int,toInt,30)
    DEFINE_PRO(QString,pacs_ae_string,pacs_ae_string,toString,"")
    DEFINE_PRO(bool,usb_enable_denoise_bool,usb_enable_denoise_bool,toBool,false)
    DEFINE_PRO(bool,usb_enable_fixed_bool,usb_enable_fixed_bool,toBool,true)

  //  Q_PROPERTY(QString      data_path       READ getData_path  WRITE setData_path    NOTIFY data_pathChanged);



//    DEFINE_PRO_NO_AUTO_GET_SET(QJsonObject,User_Info_Json,QJsonObject())


    signals:
 //DEFINE_SIG(data_path_dir_string)
        DEFINE_SIG(logined_worker_string)
        DEFINE_SIG(worker_password_string)
        DEFINE_SIG(remember_password_bool)
      DEFINE_SIG(langage_index_int)

      DEFINE_SIG(clinic_name_string)
      DEFINE_SIG(clinic_webside_string)
      DEFINE_SIG(clinic_phone_string)
      DEFINE_SIG(clinic_fax_string)
      DEFINE_SIG(clinic_address_string)
      DEFINE_SIG(clinic_post_string)
      DEFINE_SIG(clinic_country_string)
      DEFINE_SIG(clinic_province_string)
      DEFINE_SIG(clinic_city_string)

      DEFINE_SIG(pacs_ip_string)
      DEFINE_SIG(pacs_port_int)
      DEFINE_SIG(pacs_ae_string)
      DEFINE_SIG(usb_enable_denoise_bool)
      DEFINE_SIG(usb_enable_fixed_bool)



      //  void data_pathChanged();
      public:

      QString get_field_name(Fields field);
    int get_field_value(const QString & field_str);
    static Settings * get_instance();
    Q_INVOKABLE QString tran(const QString & str);
    void reload_langage();
    Q_INVOKABLE void select_langage(int index);
    Q_INVOKABLE void save_clinic_cion(const QString &file);
    Q_INVOKABLE QString get_clinic_icon_path();
    Q_INVOKABLE QString get_report_picture_dir();



private:
    Settings(QObject * parent = nullptr);
    Settings(const Settings & ) = delete;
    QMetaEnum meta;
    static Settings * _self;
    Langs langs;
    static QString * _data_path;
    static  char * * ptr ;
};

#endif // SETTINGS_H
