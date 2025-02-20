#ifndef CONFIGURE_H
#define CONFIGURE_H
#include"image_types.h"
#include<QString>
class QJsonObject;
struct Configure{

    static float Scale_float; //缩放
    static float Rotate_integer; //旋转
    static float Luminance_float; //亮度
    static float Contrast_float; //对比度
    static float Gama_float; //伽马
    static float Float_float;//浮雕
    static int Rui_Hua_integer;//锐化

    static  bool Enable_Fake_Color_Filter_BOOLEAN;//伪彩色0开启，1关闭
    static  bool Enable_Turn_Color_Filter_BOOLEAN; //反色开关

    //    static  bool Enable_High_USM__Filter_BOOLEAN;//口内龋齿开关
    //    static  bool Enable_High_DH__Filter_BOOLEAN;//口内高清开关
    //    static  bool Enable_Mouse_Detail__Filter_BOOLEAN;//口内精细开关

    static ImageFilter filter;

    static bool Enable_Hor_Mirror_BOOLEAN;
    static bool Enable_Ver_Mirror_BOOLEAN;

    static int Window_Begin;
    static int Window_End;

    static void get_default_visible_propertys(QJsonObject &obj);
    static void set_default_visible_propertys(const QJsonObject &obj);
    static void get_origin_visible_propertys(QJsonObject &obj);
    static void get_filtered_origin_visible_propertys(QJsonObject &obj, ImageFilter filter);
    static void get_visible_propertys(QJsonObject &obj, const QJsonObject &picture);
   // static void reset_factory_setting();
    static QString Line_Color; //默认线颜色

    static QString data_store_root_dir;  //数据库位置,和图片的根目录,需要再 main执行前几行代码就初始化他，后面不能在改变他
    const  static QString Origin_Image_Store_Node_Dir_Name; //原图节点目录
    const   static QString Last_State_Image_Store_Node_Dir_Name; //最后状态的图片储存节点
    const static QString Pill_Line_Image_Store_Node_Dir_Name; //柱状图储存节点
    const static QString Raw_Image_Store_Node_Dir_Name; //
    const static QString Image_Root_Node; //上面三个目录的父节点


};

#endif // CONFIGURE_H
