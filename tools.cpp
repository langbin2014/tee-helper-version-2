#include "tools.h"
#include<queue>
#include<QCoreApplication>
#include<cmath>
#include<QImage>
#include<QPainter>
#ifdef WIN32
#include <shlobj.h> // 可替换为 windows.h
#include <shellapi.h>
#endif
#include<QFile>
#include"settings.h"
#include<QThread>
#include <QDesktopServices>
#include"c++/managers/data_manager.h"
#include <QUrl>
#include<QDir>
#include<QFile>



#include"tools.h"
#include<QGuiApplication>
#include<QScreen>
#include<QImage>
#include<QPixmap>
#include<QBuffer>
#include<QJsonDocument>
#include<QJsonArray>
#include<QJsonObject>
#include<QFile>
#include<QNetworkInterface>
#include<QDir>
#include<QHostInfo>
#include<QNetworkAccessManager>
#include<QUrl>
#include<QNetworkReply>
#include"configure.h"
#include"log.h"
const QString Tools::_TAG_ = "工具类";
const bool Tools::_DEBUG_ = true;
void Tools::grap_screen(QImage &result)
{
    if(_DEBUG_)
        Log::d(_TAG_,"c++正在截屏...");
    QScreen *screen = QGuiApplication::primaryScreen();
    QPixmap  temp= screen->grabWindow(0);
    if(temp.isNull() == false)
        result = temp.toImage();
}


void Tools::bmp_2_base64_string(const QImage &img, QString &result)
{
    QByteArray ba;
    QBuffer buf(&ba);
    buf.open(QIODevice::WriteOnly);
    img.save(&buf, "BMP");
    QByteArray ba2 = ba.toBase64();
    result = QString::fromLatin1(ba2);
}

void Tools::base64_string_2_img(const QString &str, QImage &result)
{
    QByteArray arr_base64 = str.toLatin1();
    result.loadFromData(QByteArray::fromBase64(arr_base64));
}


#ifdef Q_OS_ANDROID
#include<QAndroidJniEnvironment>
#include<QAndroidJniObject>
#include<QtAndroid>
#endif
Q_INVOKABLE  QString Tools::get_mac_address()
{
#ifdef Q_OS_ANDROID
    QAndroidJniObject string = QAndroidJniObject::callStaticObjectMethod<jstring>("com/example/usb_serial/Tools", "get_mac_address");
    return string.toString();
#else
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();// 获取所有网络接口列表

    int nCnt = nets.count();

    QString strMacAddr = "";

    for(int i = 0; i < nCnt; i ++)

    {

        // 如果此网络接口被激活并且正在运行并且不是回环地址，则就是我们需要找的Mac地址

        if(nets[i].flags().testFlag(QNetworkInterface::IsUp) && nets[i].flags().testFlag(QNetworkInterface::IsRunning) && !nets[i].flags().testFlag(QNetworkInterface::IsLoopBack))

        {

            strMacAddr = nets[i].hardwareAddress();

            break;

        }

    }
    return  strMacAddr;
#endif
}



QString Tools::getPicturesDir(const QString & node_dir){
#ifdef Q_OS_ANDROID
    QAndroidJniObject string = QAndroidJniObject::callStaticObjectMethod<jstring>("com/example/usb_serial/Tools", "getPicturesDir");
    QString dir = string.toString();
    if(dir.endsWith("/"))
        dir = dir.mid(0,dir.length()-1);
    return dir;
#else
    QString image_root_node_dir =  Configure::data_store_root_dir+"/" +Configure::Image_Root_Node;
    {
        QDir dir_checker;
        if(dir_checker.exists(image_root_node_dir ) == false)
        {
            qDebug() << "file.mkdir(dir)=" << dir_checker.mkdir(image_root_node_dir);
        }
    }


    QString dir = image_root_node_dir+"/"+node_dir;;
    QDir dir_checker;
    if(dir_checker.exists(dir) == false)
    {
        qDebug() << "file.mkdir(dir)=" << dir_checker.mkdir(dir);
    }

    return dir;
#endif
}

QString Tools::create_file_name(qint64 pts, const QString &dir_node,const QString &format)
{
    const QString root_and_node_dir = Tools::getPicturesDir(dir_node);
    QDateTime current_date_time = QDateTime::fromMSecsSinceEpoch(pts);
    const  QString date_dir = current_date_time.toString("yyyy_MM_dd");
    const  QString picture_name = QString::number(pts);
    const  QString the_file_full_dir = root_and_node_dir+"/"+date_dir;
    QDir dir_checker;
    if(dir_checker.exists(the_file_full_dir) == false)
        dir_checker.mkdir(the_file_full_dir);
    const QString tail_path = Configure::Image_Root_Node +"/" + dir_node+"/"+date_dir+"/"+picture_name+ "."+format; //尾部路径，去掉数据库根目录 ,格式  名称节点/日期目录节点/文件名称
    return  tail_path;
}

QString Tools::get_today_path()
{
    QDateTime current_date_time = QDateTime::fromMSecsSinceEpoch(Tools::get_1970_ms());
    return  current_date_time.toString("yyyy_MM_dd");
}

QString Tools::get_root_path()
{
    return Configure::data_store_root_dir;
}

bool Tools::save_file_to_dir_node(const QImage &pic, const QString &name, const QStringList &dir_nodes)
{
    QString dir;
    for(int i =0; i < dir_nodes.size(); i++)
    {
        dir += dir_nodes.at(i);
        QDir dir_checker;
        if(dir_checker.exists(dir) == false)
            dir_checker.mkdir(dir);
        dir += "/";
    }
    QString path = dir + name;
    return pic.save(name,nullptr,100);
}


void Tools::fixed(QByteArray &need_fix, const QByteArray &dark, const QByteArray &light)
{
    quint16 * need_fix_ptr = (quint16 *)need_fix.data();
    const quint16 * dark_ptr =(const quint16 *) dark.data();
    const quint16 * light_ptr =(const quint16 *) light.data();
    int w = 1660;
    int h =2280;
    double dark_sum=0,light_sum = 0;
    for(int i =0; i < w*h;i++)
    {
        dark_sum += dark_ptr[i];
        light_sum += light_ptr[i];
    }
   double average_dark = dark_sum/double(w*h); //平均暗
   double average_light = light_sum/double(w*h);//平均梁
    for(int i =0; i < w*h;i++)
    {
        double dark_px  = dark_ptr[i];//暗
        double light_px = light_ptr[i];//明
        double px = need_fix_ptr[i];//当前像素
         double ret = (double(average_light-average_dark)/double(light_px-dark_px))*double(px-dark_px)+average_dark;
        if(ret < 0)
            ret = 0;
        if(ret > 4095)
            ret = 4095;
        need_fix_ptr[i] = quint16(ret);
    }
}


QString Tools::date_to_age_str(const QDateTime &date)
{
    QDateTime today = QDateTime::currentDateTime();
    int years = today.date().year() - date.date().year();
    int months = today.date().month() - date.date().month();

    if (months < 0) {
        years -= 1;
        months += 12;
    }

    QString mon_str = QString::number(months);

    QString ret = QString::number(years)+"-"+mon_str;
    return  ret;
}

QString Tools::date_to_year(const QString &date)
{
    QDate today = QDateTime::currentDateTime().date();
    QDate date2 = QDate::fromString(date,"yyyy-MM-dd");
    if(!date2.isValid())
        return "";

    int years = today.year() - date2.year();
    int months = today.month() - date2.month();

    if (months < 0) {
        years -= 1;
        months += 12;
    }

    return  QString::number(years);
}

QString Tools::date_to_month(const QString &date)
{
    QDate today = QDateTime::currentDateTime().date();
    QDate date2 = QDate::fromString(date,"yyyy-MM-dd");
    if(!date2.isValid())
        return "";

    int years = today.year() - date2.year();
    int months = today.month() - date2.month();

    if (months < 0) {
        years -= 1;
        months += 12;
    }
    return  QString::number(months);
}

void Tools::test_dir()
{
   qDebug() << QCoreApplication::applicationDirPath() +"/" +"user.ini";
   qDebug() <<  Configure::data_store_root_dir +"/" +"user.ini";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////























Tools::Tools()
{

}

QString Tools::get_ini_file_name()
{
    return Configure::data_store_root_dir +"/" +"user.ini";
   // return  QCoreApplication::applicationDirPath() +"/" +"user.ini";
}

double Tools::get_angle(double x1, double y1, double z1, double angle_x, double angle_y, double angle_z, double x2, double y2, double z2)
{
    double x11 =  x1 - angle_x;
    double y11 = y1 - angle_y;
    double z11 = z1 - angle_z;

    double x22 = x2 - angle_x;
    double y22 = y2 - angle_y;
    double z22 = z2 - angle_z;

    double dot = x11 * x22 + y11 * y22 + z11 * z22;
    double dis11 = sqrt(x11 * x11+y11*y11 + z11 * z11);
    double dis22 = sqrt(x22*x22 + y22*y22 + z22*z22);
    double cos_v = dot/(dis11*dis22);
    cos_v = acos(cos_v);
    double radius_90 = 3.1415626 /2;
    cos_v -= radius_90 ;
    double bi_li =abs(cos_v)/radius_90;
    return bi_li;
}

bool Tools::save_txt(const uchar *buf, int len, const QString &file_name)
{
    QFile f(file_name);
    bool ok = f.open(QIODevice::WriteOnly);
    if(ok)
        f.write((const char *)buf,len);
    f.flush();
    return ok;
}

void Tools::open_txt_win32(const QString &file_name)
{
    //#ifdef WIN32
    //    const wchar_t * file = reinterpret_cast<const wchar_t *>(file_name.utf16());
    //    ShellExecute(NULL,L"open",file,NULL,NULL,SW_SHOWNORMAL);
    //#endif
    //  QString webstr = "http://www.baidu.com";//设置要打开的网页
    bool openService = QDesktopServices::openUrl(QUrl(file_name));  //使用默认浏览器打开
    if(openService == false)
    {
        qDebug()<<"The openService is false";
        //Data_Manager::get_instance()->show_message("操作失败");
    }

}


QPoint get_mouse_pos_for_game_window()
{
    return QPoint();
}

double original_distance(const QPointF& pos)
{
    return std::sqrt(pos.x()*pos.x()+pos.y() * pos.y());
}

void myExit(const QString & str)
{
    qDebug() << str;
}

void Tools::make_path(const QString &path, int left)
{
    auto temp =  path;
    temp =temp.replace("\\","/");
    QStringList dirs = temp.split("/");

    QString d;
    for(int i =0; i <dirs.size() - left;i++)
    {
        d+= dirs.at(i);
        QDir dir_checker;

        if(dir_checker.exists(d) == false)
        {
            qDebug() << "file.mkdir(dir)=" << dir_checker.mkdir(d);
        }
        d +="/";
    }

}

QString Tools::to_cpp_file_path(const QString &path)
{
    QString temp = path;
    return  temp.replace("file:///","");
}

void Tools::save_list_to_xlsx(const QList<QString> list, const QString &path)
{
    QFile file(path);
    if (file.open(QFile::WriteOnly | QIODevice::Text))
    {
        QTextStream ts(&file);
        ts.setCodec("UTF-8");
        for (int i = 0; i<list.size(); i++)
        {
            ts << list.at(i) << Qt::endl;
        }

        file.close();
    }
}

QString Tools::sec_to_min_sec(int sec)
{

    QTime z_t= QTime::fromMSecsSinceStartOfDay(sec*1000);

    return z_t.toString("mm:ss");
}






