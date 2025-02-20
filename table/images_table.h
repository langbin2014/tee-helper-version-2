#ifndef IMAGES_TABLE_H
#define IMAGES_TABLE_H

#include <QObject>
#include <QMetaEnum>
#include "database.h"
class ImagesTable :public QObject,public DataBase
{
    const bool DEBUG = true;
    const QString TAG = "图片表";
    Q_OBJECT
public:
    enum class PictureSoureType{
      All,Scaner,Import,Sensor  //[ my_tr(lange_index,"Scaner"),my_tr(lange_index,"Import"),my_tr(lange_index,"Sensor")] //扫描仪,导入,传感器
    };

    enum class PictureLevel{
       All, NoLevel,Good,Normal,Bad //未分析，优秀，一般，不可接受
    };

    enum class PictureHasTeeType{
       All, NoTeeType,HasTeeType //全部，无齿形分配，有齿形
    };

    enum class Field{ //添加字段都要在 query函数里面添加对应的字段转换
        Image_ID_TEXT = 0, //图片id,是时间戳

        Patient_ID_INTEGER, //病人id,好像也可以用时间戳来定
        Image_Path_TEXT, //图片路径
        Image_Last_State_Path_TEXT,//最后状态路径
        Image_Pillar_Path_TEXT, //柱状图路径
        Describe_TEXT, //图片描述
        Tee_Type_TEXT, //牙齿类型成人和婴儿
       // Tee_from_TEXT,//牙齿类型 ???????????
        Tee_Indexs_TEXT, //包含的牙齿编号集合(1-32,和婴儿的那些牙齿),json数组[""]
        Tee_TEXT, //包含的牙齿文本编号集合(1-32,和婴儿的那些牙齿),json数组[""]
        Window_Begin_integer, //窗位起点
        Window_End_integer, //窗位终点,窗宽 = end -begin,窗位 = (bengin+end)/2
        Level_integer, //图片评分
        Source_Type_integer, //图片来源,scan,import,usb

        Scale_float, //缩放
        Rotate_integer, //旋转
        Luminance_float, //亮度
        Contrast_float, //对比度
        Gama_float, //伽马
        Float_float,//浮雕
        Rui_Hua_integer,//锐化

        Enable_Fake_Color_Filter_BOOLEAN,//伪彩色0开启，1关闭
        Enable_Turn_Color_Filter_BOOLEAN, //反色开关

//        Enable_High_USM__Filter_BOOLEAN,//口内龋齿开关
//        Enable_High_DH__Filter_BOOLEAN,//口内高清开关
//        Enable_Mouse_Detail__Filter_BOOLEAN,//口内精细开关

        Filter_Index_integer, //3个滤镜

        Enable_Hor_Mirror_BOOLEAN, //水平镜像开关
        Enable_Ver_Mirror_BOOLEAN, //垂直镜像开关

        Width_integer,//宽
        Height_integer,//高
        Image_State_Integer, //使用原图
        Pixel_Space_Hor_MM_Float, //每个像素水平间距毫米
        Pixel_Space_Ver_MM_Float, //每个像素垂直间距毫米

        Expose_Index_integer, //曝光时间索引
        Expose_Value_TEXT, //曝光时间值
        Expose_Cm_TEXT, //曝光距离
        Voltage_V_TEXT, //电压
        Electricity_A_TEXT, //电流

        // model部分
        // Lines
        // [old_value]调整轨迹
        // checked_for_export_or_delete
        // checked_for_add_to_paint_list
        // checked_for_painting

        End_DataBase_Field,

        //Current_Line_Task_integer, //线任务,废弃字段

        Enable_Draw_Rule_Bool, //尺子开关

        Checked_For_Export_Or_Delete_Bool,
        Checked_For_Add_To_Paint_List_Bool,
        Checked_For_Painting_Bool,
        Old_Value_JsonArray_Str,  //废弃
        Lines_JsonArray_Str,
        File_Name_Str, //显示的文件名
        Url_str, //url尾部
        End_Field
    };
    Q_ENUM(Field);


    static ImagesTable *getInstance();
    void create_and_init_a_picture(QJsonObject &picture_info,const QString & path,qint64 pts,qint64 patient_id, const QString & source_type)const;
    QString get_field_name(Field field);
    int get_field_value(const QString & field_str);
    bool create_and_init_a_picture_and_save_to_database(QJsonObject &obj, const QString &path, const QString &last_state_path ,
                                                        const QString &pillar_path, int width, int height, qint64 pts, qint64 patient_id,
                                                        const QString &tee_type, /*const QString &tee_from,*/ const QJsonArray &tee_indes,
                                                        const QJsonArray &tee_text, ImagesTable::PictureSoureType source_type, int w, int h,
                                                        float hor_pix_space_mm, float ver_pix_space_mm, const QString & voltage_v,
                                                        const QString & electricity_a, int expose_second_index, const QString &expose_cm,
                                                        const QString &expose_second_value);

    QJsonObject create_empty_image_info();
    void remove(const QString & id);
    void remove_this_patient_all_picture(qint64 patient_id);
    void update_from_picture_info_page_param(const QString & picture_id,const QJsonObject & info);
    void query_this_patient_picture(qint64 patient_id, QList<QJsonObject> &out);
    void query_picture(qint64 patient_id, const QString &picture_id, QJsonObject & out);
    QString query_picture_path(QString picture_id);

    bool updateImageNote(QString picture_id, QString image_note);
    bool moveImage(QString picture_id, qint64 patient_id);
protected:
    virtual void on_creating_table() override;
    virtual QString table_name()override;

private:
    ImagesTable(QObject * parent = nullptr);
    QMetaEnum meta;// = QMetaEnum::fromType<images_table::Field>();
    static ImagesTable *  _single_instance;
};

#endif // IMAGES_TABLE_H
