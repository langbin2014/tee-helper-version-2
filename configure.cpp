#include"configure.h"
//#include"tools.h"
#include"c++/table/images_table.h"
#include<QCoreApplication>
#include<QJsonObject>
float Configure::Scale_float =0.25; //缩放
float  Configure::Rotate_integer = 0.0; //旋转
float Configure::Luminance_float = 1.0; //亮度
float  Configure::Contrast_float=1.0; //对比度
float  Configure::Gama_float=1.0; //伽马
float  Configure::Float_float =0;//浮雕
int  Configure::Rui_Hua_integer = 0;//锐化

    bool Configure::Enable_Fake_Color_Filter_BOOLEAN =false;//伪彩色0开启，1关闭
    bool Configure::Enable_Turn_Color_Filter_BOOLEAN =false; //反色开关
//    bool Configure::Enable_High_USM__Filter_BOOLEAN=false;//口内龋齿开关
//    bool Configure::Enable_High_DH__Filter_BOOLEAN=false;//口内高清开关
//    bool Configure::Enable_Mouse_Detail__Filter_BOOLEAN=false;//口内精细开关
    ImageFilter Configure::filter = ImageFilter::NoFilter;

    bool Configure::Enable_Hor_Mirror_BOOLEAN = false;
    bool Configure::Enable_Ver_Mirror_BOOLEAN = false;

   int Configure::Window_Begin = 0;
    int Configure::Window_End = 65535;

    QString Configure::data_store_root_dir =  "D:/temp";// QCoreApplication::applicationDirPath()+"/";
   const  QString Configure::Origin_Image_Store_Node_Dir_Name = "origin_image"; //原图节点目录
    const  QString Configure::Last_State_Image_Store_Node_Dir_Name = "last_state_image"; //最后状态的图片储存节点
    const  QString Configure::Pill_Line_Image_Store_Node_Dir_Name = "pill_line_image"; //柱状图储存节点
    const  QString Configure::Image_Root_Node = "Images";
   const  QString Configure::Raw_Image_Store_Node_Dir_Name = "raw"; //柱状图储存节点


    void Configure::get_default_visible_propertys(QJsonObject &obj)
    {
        ImagesTable * tb = ImagesTable::getInstance();

        QString rotate_key = tb->get_field_name(ImagesTable::Field::Rotate_integer);
        QString brightness_key = tb->get_field_name(ImagesTable::Field::Luminance_float);
        QString const_key = tb->get_field_name(ImagesTable::Field::Contrast_float);
        QString gama_key = tb->get_field_name(ImagesTable::Field::Gama_float);

        QString float_key = tb->get_field_name(ImagesTable::Field::Float_float);
        QString rui_hua_key = tb->get_field_name(ImagesTable::Field::Rui_Hua_integer);
        QString enable_fake_color_key = tb->get_field_name(ImagesTable::Field::Enable_Fake_Color_Filter_BOOLEAN);
        QString enable_turn_key = tb->get_field_name(ImagesTable::Field::Enable_Turn_Color_Filter_BOOLEAN);

        QString filter_key = tb->get_field_name(ImagesTable::Field::Filter_Index_integer);
        QString enable_hor_mir = tb->get_field_name(ImagesTable::Field::Enable_Hor_Mirror_BOOLEAN);
        QString enable_ver_mir = tb->get_field_name(ImagesTable::Field::Enable_Ver_Mirror_BOOLEAN);

        QString begin_key = tb->get_field_name(ImagesTable::Field::Window_Begin_integer);
        QString end_key = tb->get_field_name(ImagesTable::Field::Window_End_integer);

        obj[rotate_key] = Rotate_integer;
        obj[brightness_key] = Luminance_float;
        obj[const_key] = Contrast_float;

        obj[gama_key]  = Gama_float;
        obj[float_key] = Float_float;
        obj[rui_hua_key] = Rui_Hua_integer;

        obj[enable_fake_color_key] = Enable_Fake_Color_Filter_BOOLEAN;
        obj[enable_turn_key] = Enable_Turn_Color_Filter_BOOLEAN;
        obj[filter_key] = int(filter);
        obj[enable_hor_mir] = Enable_Hor_Mirror_BOOLEAN;
        obj[enable_ver_mir] = Enable_Ver_Mirror_BOOLEAN;
        obj[begin_key] = Window_Begin;
        obj[end_key] = Window_End;
    }

    void Configure::set_default_visible_propertys(const QJsonObject &obj)
    {
        ImagesTable * tb = ImagesTable::getInstance();

        QString rotate_key = tb->get_field_name(ImagesTable::Field::Rotate_integer);
        QString brightness_key = tb->get_field_name(ImagesTable::Field::Luminance_float);
        QString const_key = tb->get_field_name(ImagesTable::Field::Contrast_float);
        QString gama_key = tb->get_field_name(ImagesTable::Field::Gama_float);

        QString float_key = tb->get_field_name(ImagesTable::Field::Float_float);
        QString rui_hua_key = tb->get_field_name(ImagesTable::Field::Rui_Hua_integer);
        QString enable_fake_color_key = tb->get_field_name(ImagesTable::Field::Enable_Fake_Color_Filter_BOOLEAN);
        QString enable_turn_key = tb->get_field_name(ImagesTable::Field::Enable_Turn_Color_Filter_BOOLEAN);

        QString filter_key = tb->get_field_name(ImagesTable::Field::Filter_Index_integer);
        QString enable_hor_mir = tb->get_field_name(ImagesTable::Field::Enable_Hor_Mirror_BOOLEAN);
        QString enable_ver_mir = tb->get_field_name(ImagesTable::Field::Enable_Ver_Mirror_BOOLEAN);

        QString begin_key = tb->get_field_name(ImagesTable::Field::Window_Begin_integer);
        QString end_key = tb->get_field_name(ImagesTable::Field::Window_End_integer);

        Rotate_integer = obj[rotate_key].toInt();
        Luminance_float = obj[brightness_key].toDouble();
         Contrast_float = obj[const_key].toDouble();

        Gama_float = obj[gama_key].toDouble();
        Float_float = obj[float_key].toDouble();
        Rui_Hua_integer = obj[rui_hua_key].toInt();

         Enable_Fake_Color_Filter_BOOLEAN = obj[enable_fake_color_key].toBool();
        Enable_Turn_Color_Filter_BOOLEAN = obj[enable_turn_key].toBool();
        filter =ImageFilter(obj[filter_key].toInt());
         Enable_Hor_Mirror_BOOLEAN =obj[enable_hor_mir].toBool();
        Enable_Ver_Mirror_BOOLEAN = obj[enable_ver_mir].toBool();
        Window_Begin = obj[begin_key].toInt();
        Window_End = obj[end_key].toInt();
    }

    void Configure::get_origin_visible_propertys(QJsonObject & obj)
    {
        ImagesTable * tb = ImagesTable::getInstance();

        QString rotate_key = tb->get_field_name(ImagesTable::Field::Rotate_integer);
        QString brightness_key = tb->get_field_name(ImagesTable::Field::Luminance_float);
        QString const_key = tb->get_field_name(ImagesTable::Field::Contrast_float);
        QString gama_key = tb->get_field_name(ImagesTable::Field::Gama_float);

        QString float_key = tb->get_field_name(ImagesTable::Field::Float_float);
        QString rui_hua_key = tb->get_field_name(ImagesTable::Field::Rui_Hua_integer);
        QString enable_fake_color_key = tb->get_field_name(ImagesTable::Field::Enable_Fake_Color_Filter_BOOLEAN);
        QString enable_turn_key = tb->get_field_name(ImagesTable::Field::Enable_Turn_Color_Filter_BOOLEAN);

        QString filter_key = tb->get_field_name(ImagesTable::Field::Filter_Index_integer);
        QString enable_hor_mir = tb->get_field_name(ImagesTable::Field::Enable_Hor_Mirror_BOOLEAN);
        QString enable_ver_mir = tb->get_field_name(ImagesTable::Field::Enable_Ver_Mirror_BOOLEAN);

        QString begin_key = tb->get_field_name(ImagesTable::Field::Window_Begin_integer);
        QString end_key = tb->get_field_name(ImagesTable::Field::Window_End_integer);

        obj[rotate_key] = 0;
        obj[brightness_key] = 1.0;
        obj[const_key] = 1.0;
        obj[gama_key]  = 1.0;
        obj[float_key] = 0.0;
        obj[rui_hua_key] = 0.0;
        obj[enable_fake_color_key] = false;
        obj[enable_turn_key] = false;
        obj[filter_key] = int(ImageFilter::NoFilter);
        obj[enable_hor_mir] = false;
        obj[enable_ver_mir] = false;
        obj[begin_key] = 0;
        obj[end_key] = 65535;
    }

    void Configure::get_filtered_origin_visible_propertys(QJsonObject &obj,ImageFilter filter)
    {
        ImagesTable * tb = ImagesTable::getInstance();
        QString brightness_key = tb->get_field_name(ImagesTable::Field::Luminance_float);
        QString const_key = tb->get_field_name(ImagesTable::Field::Contrast_float);
        QString gama_key = tb->get_field_name(ImagesTable::Field::Gama_float);

        QString float_key = tb->get_field_name(ImagesTable::Field::Float_float);
        QString rui_hua_key = tb->get_field_name(ImagesTable::Field::Rui_Hua_integer);
        QString enable_fake_color_key = tb->get_field_name(ImagesTable::Field::Enable_Fake_Color_Filter_BOOLEAN);
        QString enable_turn_key = tb->get_field_name(ImagesTable::Field::Enable_Turn_Color_Filter_BOOLEAN);

        QString begin_key = tb->get_field_name(ImagesTable::Field::Window_Begin_integer);
        QString end_key = tb->get_field_name(ImagesTable::Field::Window_End_integer);

        obj[brightness_key] = 1.0;
        obj[const_key] = filter == ImageFilter::NoFilter ? 1.0 : 1.0;//1.14;
        obj[gama_key]  = 1.0;
        obj[float_key] = 0.0;
        obj[rui_hua_key] = 0.0;
        obj[enable_fake_color_key] = false;
        obj[enable_turn_key] = false;
        obj[begin_key] = 0;
        obj[end_key] = 65535;
    }

    void Configure::get_visible_propertys(QJsonObject &obj, const QJsonObject &picture)
    {
        ImagesTable * tb = ImagesTable::getInstance();

        QString rotate_key = tb->get_field_name(ImagesTable::Field::Rotate_integer);
        QString brightness_key = tb->get_field_name(ImagesTable::Field::Luminance_float);
        QString const_key = tb->get_field_name(ImagesTable::Field::Contrast_float);
        QString gama_key = tb->get_field_name(ImagesTable::Field::Gama_float);

        QString float_key = tb->get_field_name(ImagesTable::Field::Float_float);
        QString rui_hua_key = tb->get_field_name(ImagesTable::Field::Rui_Hua_integer);
        QString enable_fake_color_key = tb->get_field_name(ImagesTable::Field::Enable_Fake_Color_Filter_BOOLEAN);
        QString enable_turn_key = tb->get_field_name(ImagesTable::Field::Enable_Turn_Color_Filter_BOOLEAN);

        QString filter_key = tb->get_field_name(ImagesTable::Field::Filter_Index_integer);
        QString enable_hor_mir = tb->get_field_name(ImagesTable::Field::Enable_Hor_Mirror_BOOLEAN);
        QString enable_ver_mir = tb->get_field_name(ImagesTable::Field::Enable_Ver_Mirror_BOOLEAN);

        QString begin_key = tb->get_field_name(ImagesTable::Field::Window_Begin_integer);
        QString end_key = tb->get_field_name(ImagesTable::Field::Window_End_integer);

        obj[rotate_key] = picture[rotate_key];
        obj[brightness_key] = picture[brightness_key];
        obj[const_key] = picture[const_key];
        obj[gama_key]  = picture[gama_key];
        obj[float_key] = picture[float_key];
        obj[rui_hua_key] = picture[rui_hua_key];
        obj[enable_fake_color_key] = picture[enable_fake_color_key];
        obj[enable_turn_key] = picture[enable_turn_key];
        obj[filter_key] = picture[filter_key];
        obj[enable_hor_mir] = picture[enable_hor_mir];
        obj[enable_ver_mir] = picture[enable_ver_mir];
        obj[begin_key] = picture[begin_key];
        obj[end_key] = picture[end_key];
    }

//    void Configure::reset_factory_setting()
//    {
//       Contrast_float = 1.5299999713897705;
//       Enable_Fake_Color_Filter_BOOLEAN =false;
//       Enable_Hor_Mirror_BOOLEAN =false;
//       Enable_Turn_Color_Filter_BOOLEAN=false;
//       Enable_Ver_Mirror_BOOLEAN=false;
//       filter=ImageFilter(2);
//       Float_float=0;
//       Gama_float=0.949999988079071;
//       Luminance_float=0.7599999904632568;
//       Rotate_integer=90;
//       Rui_Hua_integer=0;
//       Window_Begin=0;
//       Window_End=65535;

 //   }

     QString Configure::Line_Color = "#00FF00";
