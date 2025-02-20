#include "settings.h"
#include<QSettings>
#include<QCoreApplication>
#include"tools.h"
#include<QTimer>
#include <QDir>
#include<QImage>
#include <QSettings>
#include"configure.h"
#define DELETE_PTR(p){ if(p) { delete p; p = NULL; } } // 指针释放操作
#define AUTO_RUN_KEY "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"
Settings * Settings::_self = nullptr;
QString * Settings::_data_path = new QString();
 char * * Settings::ptr =  nullptr;
//设置程序自启动 appPath程序路径
void SetAppAutoRun(bool bAutoRun)
{
    QString application_name = QCoreApplication::applicationName(); //获取应用名称
    QSettings* pSettings = new QSettings(AUTO_RUN_KEY, QSettings::NativeFormat); //创建QSetting, 需要添加QSetting头文件
    if (bAutoRun)
    {
        QString application_path = QCoreApplication::applicationFilePath(); //找到应用的目录
        pSettings->setValue(application_name, application_path.replace("/", "\\")); //写入注册表
    }
    else
    {
        pSettings->remove(application_name);
    }

    DELETE_PTR(pSettings);
}

void SetProcessAutoRunSelf(const QString &appPath,bool enable)
{

    //注册表路径需要使用双反斜杠，如果是32位系统，要使用QSettings::Registry32Format
    QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                       QSettings::Registry64Format);

    //以程序名称作为注册表中的键
    //根据键获取对应的值（程序路径）
    QFileInfo fInfo(appPath);
    QString name = fInfo.baseName();
    QString path = settings.value(name).toString();

    //如果注册表中的路径和当前程序路径不一样，
    //则表示没有设置自启动或自启动程序已经更换了路径
    //toNativeSeparators的意思是将"/"替换为"\"
    QString newPath = QDir::toNativeSeparators(appPath);
    if (enable)
    {
        if(path != newPath)
            settings.setValue(name, newPath);
    }
    else
    {
           settings.remove(name);
    }
}

Settings::Settings(QObject *parent):QObject(parent),meta(QMetaEnum::fromType<Settings::Fields>())
{

   // QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat);
   // QString dir = QCoreApplication::applicationDirPath();

//    connect(this,&Settings::auto_run_pc_soft_boolChanged,this,[this]{
//        SetAppAutoRun(m_auto_run_pc_soft_bool);
//         //  SetProcessAutoRunSelf(QCoreApplication::applicationFilePath(),m_auto_run_pc_soft_bool);
//    });
}



Settings *Settings::get_instance()
{
    if(_self == nullptr)
    {
        _self = new Settings();
        _self->langs.set_index(_self->get_langage_index_int());
    }
    return _self;
}

QString Settings::tran(const QString &  key)
{
    int index = langs.get_index();
    if(key == "预览")
    {
        if(index == 0)
            return "预\n\n\n览";
        else if(index == 1)
            return "P\nr\ne\nv\ni\ne\nw";
        else if(index == 2)
            return  "P\nr\ne\nv\ni\na";
            else
            return key;

    }
    else if(key == "采集页")
    {
        if(index == 0)
            return "采\n\n\n集";
        else if(index == 1)
            return "C\no\nl\nl\ne\nc\nt";
        else if(index == 2)
            return  "R\ne\nc\no\nl\ne\nc\nc\ni\nó\nn";
            else

             return key;
    }
    else if(key == "左侧")
    {
        if(index == 0)
            return "左\n侧";
        else if(index == 1)
            return "L\ne\nf\nt";
        else
            return "左\n侧";

    }
    else if(key == "右侧")
    {
        if(index == 0)
            return "右\n侧";
        else if(index == 1)
            return "R\ni\ng\nh\nt";
        else
             return "右\n侧";
    }
    else
        return langs.my_tr(key);
}

void Settings::reload_langage()
{
    langs.clear();
    int temp = m_langage_index_int;
    select_langage(0);
    QTimer::singleShot(500,[this,temp]{
        select_langage(temp);
    });
    // langage_index_intChanged();
}

void Settings::select_langage(int index)
{
    langs.set_index(index);
    set_langage_index_int(index);
}

void Settings::save_clinic_cion(const QString &file)
{
    QString temp = file;
    temp =   temp.replace("file:///","");
    QImage pic(temp);
    QString local_path = Configure::data_store_root_dir+"/clinic_icon.png";
    pic.save(local_path);

}

QString Settings::get_clinic_icon_path()
{
    return "file:///" +Configure::data_store_root_dir+"/clinic_icon.png";
}

QString Settings::get_report_picture_dir()
{
    return Configure::data_store_root_dir+"/report_pictures";
}



QString Settings::get_field_name(Settings::Fields field)
{
    return meta.valueToKey(int(field));
}

int Settings::get_field_value(const QString &field_str)
{
    return meta.keyToValue(field_str.toLatin1().data());
}


