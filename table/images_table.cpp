#include "images_table.h"
#include "c++/tools.h"
#include "c++/configure.h"
#include <QSqlError>
//#include "global.h"
#include"c++/log.h"
ImagesTable * ImagesTable::_single_instance = nullptr;
ImagesTable::ImagesTable(QObject *parent):QObject(parent),meta(QMetaEnum::fromType<ImagesTable::Field>()),DataBase()
{
    check_table_or_create_it();
}

ImagesTable *ImagesTable::getInstance()
{
    if(_single_instance == nullptr)
        _single_instance = new ImagesTable;
    return  _single_instance;
}

bool ImagesTable::create_and_init_a_picture_and_save_to_database(QJsonObject &obj, const QString &path,
                                                                 const QString &last_state_path ,const QString & pillar_path,int width,
                                                                 int height,qint64 pts, qint64 patient_id, const QString &tee_type,
                                                                 const QJsonArray &tee_indes, const QJsonArray &tee_text,
                                                                 ImagesTable::PictureSoureType  source_type,int w, int h,float hor_pix_space_mm,
                                                                 float ver_pix_space_mm,const QString & voltage_v, const QString & electricity_a,
                                                                 int expose_second_index, const QString & expose_cm,
                                                                 const QString & expose_second_value)
{
    QMutexLocker lock(_lock);
    QString head = "INSERT INTO " + table_name() +" ";
    QString fields;
    QString fields_alias;
    for(int i =0;i < int(Field::End_DataBase_Field);i++)
    {
        QString field = meta.valueToKey(i);
        if(i != int(Field::End_DataBase_Field)-1)
        {
            fields += field+", ";
            fields_alias += ":"+field+", ";
        }
        else
        {
            fields += field;
            fields_alias += ":"+field;
        }
    }
    QString sql = head +"(" +fields +")" +" VALUES ("+fields_alias+")";

    if(DEBUG)
        Log::d(TAG,"生成的追加sql:"+sql);
    QSqlQuery query(*_conn);
    query.prepare(sql);
    //("INSERT INTO T_USER_MANAGE (UsreName, IP, Port, PassWord, Type) VALUES (:UsreName, :IP, :Port, :PassWord, :Type)");
    QString field;
    field  =  meta.valueToKey(int(Field::Image_ID_TEXT));
    QString pts_str = QString::number(pts);
    obj[field] = pts_str;
    query.bindValue(":"+field,pts_str);

    field   =  meta.valueToKey(int(Field::Patient_ID_INTEGER));
    obj[field] = patient_id;
    query.bindValue(":"+field,patient_id);

    field   =  meta.valueToKey(int(Field::Image_Path_TEXT));
    obj[field] =Tools::get_root_path()+"/"+  path;
    query.bindValue(":"+field,path);

    field   =  meta.valueToKey(int(Field::Image_Last_State_Path_TEXT));
    obj[field] =Tools::get_root_path()+"/"+  last_state_path;
    query.bindValue(":"+field,last_state_path);

    field   =  meta.valueToKey(int(Field::Image_Pillar_Path_TEXT));
    obj[field] =Tools::get_root_path()+"/"+  pillar_path;
    query.bindValue(":"+field,pillar_path);


    field   =  meta.valueToKey(int(Field::Describe_TEXT));
    obj[field] = "";
    query.bindValue(":"+field,"");

    field   =  meta.valueToKey(int(Field::Tee_Type_TEXT));
    obj[field] = tee_type;
    query.bindValue(":"+field,tee_type);

    QString tee_index_str;
    Tools::jsonObj2String(tee_indes,tee_index_str);
    field   =  meta.valueToKey(int(Field::Tee_Indexs_TEXT));
    obj[field] = tee_index_str;
    query.bindValue(":"+field,tee_index_str);

   // field   =  meta.valueToKey(int(Field::Tee_from));
   // obj[field] = tee_from;
   // query.bindValue(":"+field,tee_from);

    QString tee_text_str;
    Tools::jsonObj2String(tee_text,tee_text_str);
    field   =  meta.valueToKey(int(Field::Tee_TEXT));
    obj[field] = tee_text_str;
    query.bindValue(":"+field,tee_text_str);

    field   =  meta.valueToKey(int(Field::Window_Begin_integer));
    obj[field] = width;
    query.bindValue(":"+field,width);

    field   =  meta.valueToKey(int(Field::Window_End_integer));
    obj[field] = height;
    query.bindValue(":"+field,height);

    field   =  meta.valueToKey(int(Field::Level_integer));
    obj[field] = 0; //0 是无评价，1,2,3是3个等级评价
    query.bindValue(":"+field,0);

    field   =  meta.valueToKey(int(Field::Source_Type_integer));
    obj[field] = int(source_type); //图片来源
    query.bindValue(":"+field,int(source_type));

    field   =  meta.valueToKey(int(Field::Scale_float));
    obj[field] = Configure::Scale_float;
    query.bindValue(":"+field,Configure::Scale_float);

    field   =  meta.valueToKey(int(Field::Rotate_integer));
    obj[field] = Configure::Rotate_integer;
    query.bindValue(":"+field,Configure::Rotate_integer);

    field   =  meta.valueToKey(int(Field::Luminance_float));
    obj[field] = Configure::Luminance_float;
    query.bindValue(":"+field,Configure::Luminance_float);

    field   =  meta.valueToKey(int(Field::Contrast_float));
    obj[field] = Configure::Contrast_float;
    query.bindValue(":"+field,Configure::Contrast_float);

    field   =  meta.valueToKey(int(Field::Gama_float));
    obj[field] = Configure::Gama_float;
    query.bindValue(":"+field,Configure::Gama_float);

    field   =  meta.valueToKey(int(Field::Float_float));
    obj[field] = Configure::Float_float;
    query.bindValue(":"+field,Configure::Float_float);

    field   =  meta.valueToKey(int(Field::Rui_Hua_integer));
    obj[field] = Configure::Rui_Hua_integer;
    query.bindValue(":"+field,Configure::Rui_Hua_integer);


    field   =  meta.valueToKey(int(Field::Enable_Fake_Color_Filter_BOOLEAN));
    obj[field] = Configure::Enable_Fake_Color_Filter_BOOLEAN;
    query.bindValue(":"+field,Configure::Enable_Fake_Color_Filter_BOOLEAN);


    field   =  meta.valueToKey(int(Field::Enable_Turn_Color_Filter_BOOLEAN));
    obj[field] = Configure::Enable_Turn_Color_Filter_BOOLEAN;
    query.bindValue(":"+field,Configure::Enable_Turn_Color_Filter_BOOLEAN);

//    field   =  meta.valueToKey(int(Field::Enable_High_USM__Filter_BOOLEAN));
//    obj[field] = Configure::Enable_High_USM__Filter_BOOLEAN;
//    query.bindValue(":"+field,Configure::Enable_High_USM__Filter_BOOLEAN);

//    field   =  meta.valueToKey(int(Field::Enable_High_DH__Filter_BOOLEAN));
//    obj[field] = Configure::Enable_High_DH__Filter_BOOLEAN;
//    query.bindValue(":"+field,Configure::Enable_High_DH__Filter_BOOLEAN);

//    field   =  meta.valueToKey(int(Field::Enable_Mouse_Detail__Filter_BOOLEAN));
//    obj[field] = Configure::Enable_Mouse_Detail__Filter_BOOLEAN;
//    query.bindValue(":"+field,Configure::Enable_Mouse_Detail__Filter_BOOLEAN);

        field   =  meta.valueToKey(int(Field::Filter_Index_integer));
        obj[field] = int(Configure::filter);
        query.bindValue(":"+field,int(Configure::filter));

    field   =  meta.valueToKey(int(Field::Enable_Hor_Mirror_BOOLEAN));
    obj[field] = Configure::Enable_Hor_Mirror_BOOLEAN;
    query.bindValue(":"+field,Configure::Enable_Hor_Mirror_BOOLEAN);

    field   =  meta.valueToKey(int(Field::Enable_Ver_Mirror_BOOLEAN));
    obj[field] = Configure::Enable_Ver_Mirror_BOOLEAN;
    query.bindValue(":"+field,Configure::Enable_Ver_Mirror_BOOLEAN);


    field   =  meta.valueToKey(int(Field::Width_integer));
    obj[field] = w;
    query.bindValue(":"+field,w);

    field   =  meta.valueToKey(int(Field::Height_integer));
    obj[field] = h;
    query.bindValue(":"+field,h);

    field   =  meta.valueToKey(int(Field::Image_State_Integer));
    obj[field] = int(ImageState::Last);
    query.bindValue(":"+field,int(ImageState::Last));

    field   =  meta.valueToKey(int(Field::Pixel_Space_Hor_MM_Float));
    obj[field] = hor_pix_space_mm;
    query.bindValue(":"+field,hor_pix_space_mm);

    field   =  meta.valueToKey(int(Field::Pixel_Space_Ver_MM_Float));
    obj[field] = ver_pix_space_mm;
    query.bindValue(":"+field,ver_pix_space_mm);

    field   =  meta.valueToKey(int(Field::Expose_Index_integer));
    obj[field] = expose_second_index;
    query.bindValue(":"+field,expose_second_index);

    field   =  meta.valueToKey(int(Field::Expose_Value_TEXT));
    obj[field] = expose_second_value;
    query.bindValue(":"+field, expose_second_value);

    field   =  meta.valueToKey(int(Field::Expose_Cm_TEXT));
    obj[field] = expose_cm;
    query.bindValue(":"+field,expose_cm);

    field   =  meta.valueToKey(int(Field::Voltage_V_TEXT));
    obj[field] = voltage_v;
    query.bindValue(":"+field,voltage_v);

    field   =  meta.valueToKey(int(Field::Electricity_A_TEXT));
    obj[field] = electricity_a;
    query.bindValue(":"+field,electricity_a);
    //不属于数据库的用户操作状态字段

//    field   =  meta.valueToKey(int(Field::Current_Line_Task_integer));
//    obj[field] =  int(LineTask::NoLineTask);


    field   =  meta.valueToKey(int(Field::Enable_Draw_Rule_Bool));
    obj[field] = false;


    field   =  meta.valueToKey(int(Field::Checked_For_Export_Or_Delete_Bool));
    obj[field] = false;

    field   =  meta.valueToKey(int(Field::Checked_For_Add_To_Paint_List_Bool));
    obj[field] = false;

    field   =  meta.valueToKey(int(Field::Checked_For_Painting_Bool));
    obj[field] = false;

    field   =  meta.valueToKey(int(Field::Old_Value_JsonArray_Str));
    obj[field] = "[]";

    field   =  meta.valueToKey(int(Field::Lines_JsonArray_Str));
    obj[field] = "[]";

    field   =  meta.valueToKey(int(Field::Url_str));
    obj[field] = "image://editing_images/"+ pts_str+ "/-1";

    field   =  meta.valueToKey(int(Field::File_Name_Str));
    QDateTime time = QDateTime::fromMSecsSinceEpoch(pts); //时间戳-毫秒级
    QString name = time.toString("yyyy-MM-dd hh:mm:ss");
    obj[field] = name ;

    bool ok =  query.exec();
    if(!ok)
        Log::e(TAG,"新增图片保存失败:"+query.lastError().text());
    return ok;
}

QJsonObject ImagesTable::create_empty_image_info()
{

    QJsonObject obj;
    QString field;
    field  =  meta.valueToKey(int(Field::Image_ID_TEXT));
    QString pts_str = "-1";
    obj[field] = pts_str;

    field   =  meta.valueToKey(int(Field::Patient_ID_INTEGER));
    obj[field] = -1;

    field   =  meta.valueToKey(int(Field::Image_Path_TEXT));
    obj[field] ="";


    field   =  meta.valueToKey(int(Field::Image_Last_State_Path_TEXT));
    obj[field] ="";


    field   =  meta.valueToKey(int(Field::Image_Pillar_Path_TEXT));
    obj[field] ="";


    field   =  meta.valueToKey(int(Field::Describe_TEXT));
    obj[field] = "";

    field   =  meta.valueToKey(int(Field::Tee_Type_TEXT));
    obj[field] = "kid";



    field   =  meta.valueToKey(int(Field::Tee_Indexs_TEXT));
    obj[field] = "[]";

   // field   =  meta.valueToKey(int(Field::Tee_from));
   // obj[field] = tee_from;
   // query.bindValue(":"+field,tee_from);


    field   =  meta.valueToKey(int(Field::Tee_TEXT));
    obj[field] = "[]";

    field   =  meta.valueToKey(int(Field::Window_Begin_integer));
    obj[field] =0;


    field   =  meta.valueToKey(int(Field::Window_End_integer));
    obj[field] = 65535;

    field   =  meta.valueToKey(int(Field::Level_integer));
    obj[field] = 0; //0 是无评价，1,2,3是3个等级评价


    field   =  meta.valueToKey(int(Field::Source_Type_integer));
    obj[field] = int(2); //图片来源

    field   =  meta.valueToKey(int(Field::Scale_float));
    obj[field] = Configure::Scale_float;


    field   =  meta.valueToKey(int(Field::Rotate_integer));
    obj[field] = Configure::Rotate_integer;

    field   =  meta.valueToKey(int(Field::Luminance_float));
    obj[field] = Configure::Luminance_float;

    field   =  meta.valueToKey(int(Field::Contrast_float));
    obj[field] = Configure::Contrast_float;

    field   =  meta.valueToKey(int(Field::Gama_float));
    obj[field] = Configure::Gama_float;

    field   =  meta.valueToKey(int(Field::Float_float));
    obj[field] = Configure::Float_float;

    field   =  meta.valueToKey(int(Field::Rui_Hua_integer));
    obj[field] = Configure::Rui_Hua_integer;


    field   =  meta.valueToKey(int(Field::Enable_Fake_Color_Filter_BOOLEAN));
    obj[field] = Configure::Enable_Fake_Color_Filter_BOOLEAN;


    field   =  meta.valueToKey(int(Field::Enable_Turn_Color_Filter_BOOLEAN));
    obj[field] = Configure::Enable_Turn_Color_Filter_BOOLEAN;

//    field   =  meta.valueToKey(int(Field::Enable_High_USM__Filter_BOOLEAN));
//    obj[field] = Configure::Enable_High_USM__Filter_BOOLEAN;
//    query.bindValue(":"+field,Configure::Enable_High_USM__Filter_BOOLEAN);

//    field   =  meta.valueToKey(int(Field::Enable_High_DH__Filter_BOOLEAN));
//    obj[field] = Configure::Enable_High_DH__Filter_BOOLEAN;
//    query.bindValue(":"+field,Configure::Enable_High_DH__Filter_BOOLEAN);

//    field   =  meta.valueToKey(int(Field::Enable_Mouse_Detail__Filter_BOOLEAN));
//    obj[field] = Configure::Enable_Mouse_Detail__Filter_BOOLEAN;
//    query.bindValue(":"+field,Configure::Enable_Mouse_Detail__Filter_BOOLEAN);

        field   =  meta.valueToKey(int(Field::Filter_Index_integer));
        obj[field] = int(Configure::filter);

    field   =  meta.valueToKey(int(Field::Enable_Hor_Mirror_BOOLEAN));
    obj[field] = Configure::Enable_Hor_Mirror_BOOLEAN;

    field   =  meta.valueToKey(int(Field::Enable_Ver_Mirror_BOOLEAN));
    obj[field] = Configure::Enable_Ver_Mirror_BOOLEAN;


    field   =  meta.valueToKey(int(Field::Width_integer));
    obj[field] = 1;

    field   =  meta.valueToKey(int(Field::Height_integer));
    obj[field] = 1;

    field   =  meta.valueToKey(int(Field::Image_State_Integer));
    obj[field] = int(ImageState::Last);

    field   =  meta.valueToKey(int(Field::Pixel_Space_Hor_MM_Float));
    obj[field] = 1.0;

    field   =  meta.valueToKey(int(Field::Pixel_Space_Ver_MM_Float));
    obj[field] = 1.0;

    field   =  meta.valueToKey(int(Field::Expose_Index_integer));
    obj[field] = 0;

    field   =  meta.valueToKey(int(Field::Expose_Value_TEXT));
    obj[field] = "2.0";

    field   =  meta.valueToKey(int(Field::Expose_Cm_TEXT));
    obj[field] = "2.0";

    field   =  meta.valueToKey(int(Field::Voltage_V_TEXT));
    obj[field] = "2.0";

    field   =  meta.valueToKey(int(Field::Electricity_A_TEXT));
    obj[field] = "2.0";

    //不属于数据库的用户操作状态字段

//    field   =  meta.valueToKey(int(Field::Current_Line_Task_integer));
//    obj[field] =  int(LineTask::NoLineTask);


    field   =  meta.valueToKey(int(Field::Enable_Draw_Rule_Bool));
    obj[field] = false;


    field   =  meta.valueToKey(int(Field::Checked_For_Export_Or_Delete_Bool));
    obj[field] = false;

    field   =  meta.valueToKey(int(Field::Checked_For_Add_To_Paint_List_Bool));
    obj[field] = false;

    field   =  meta.valueToKey(int(Field::Checked_For_Painting_Bool));
    obj[field] = false;

    field   =  meta.valueToKey(int(Field::Old_Value_JsonArray_Str));
    obj[field] = "[]";

    field   =  meta.valueToKey(int(Field::Lines_JsonArray_Str));
    obj[field] = "[]";

    field   =  meta.valueToKey(int(Field::Url_str));
    obj[field] = "image://editing_images/"+ pts_str+ "/-1";

    field   =  meta.valueToKey(int(Field::File_Name_Str));
    QDateTime time = QDateTime::fromMSecsSinceEpoch(Tools::get_1970_ms()); //时间戳-毫秒级
    QString name = time.toString("yyyy-MM-dd hh:mm:ss");
    obj[field] = name ;

    return obj;
}

void ImagesTable::remove(const QString &id)
{

    if(DEBUG)
        Log::d(TAG,"正在删除数据库图片:"+id);
    QSqlQuery q(*_conn);
    QString key = get_field_name(Field::Image_ID_TEXT);
    QString template_str = "delete  from  "+ table_name() + " where " + key + " = " + id;
    bool ok =  q.exec(template_str);
    if(!ok)
        Log::e(TAG,"删除图片错误:"+q.lastError().text());
}

void ImagesTable::remove_this_patient_all_picture(qint64 patient_id)
{
    QString id_str = QString::number(patient_id);
    if(DEBUG)
        Log::d(TAG,"正在病人数据库所有图片，病人id:"+id_str);
    QSqlQuery q(*_conn);
    QString key = get_field_name(Field::Patient_ID_INTEGER);
    QString template_str = "delete  from  "+ table_name() + " where " + key + " = " + id_str;
    bool ok =  q.exec(template_str);
    if(!ok)
        Log::e(TAG,"删除所有图片错误:"+q.lastError().text());
}

void ImagesTable::update_from_picture_info_page_param(const QString &picture_id, const QJsonObject &info)
{
    if(DEBUG)
    {
        QString ret;
        Tools::jsonObj2String(info,ret);
        Log::d(TAG,"正在更新图片信息:"+ret);
    }

    QString key_value;
    int i =0;
    int size = info.size();
    for(auto it = info.begin();it != info.end();it++)
    {

        key_value += it.key() + " = :" +it.key() +((i != size -1) ? ", " : "");
        i++;
    }
    QSqlQuery q(*_conn);
    QString key = get_field_name(Field::Image_ID_TEXT);
    QString template_str = "update  "+ table_name() + " set "+key_value +" where " + key + " = " + picture_id;
    q.prepare(template_str);
    qDebug()<<"update_from_picture_info_page_param___________:"<<template_str;
    for(auto it = info.begin();it != info.end();it++)
    {
        qDebug()<<"update_from_picture_info_page_param___________it.key():"<<it.key()<<"it.value().toVariant():"<<it.value().toVariant();
        q.bindValue(":"+it.key(),it.value().toVariant());
    }
    bool ok = q.exec();
    if(!ok)
        Log::e(TAG,"更新图片的错误:"+q.lastError().text());
}

QString ImagesTable::get_field_name(ImagesTable::Field field)
{
    return meta.valueToKey(int(field));
}

int ImagesTable::get_field_value(const QString &field_str)
{
    return meta.keyToValue(field_str.toLatin1().data());
}

void ImagesTable::on_creating_table()
{
  //  QMutexLocker locker(_lock);
    QString head = QString("CREATE TABLE %1 (").arg(table_name());
    QString fields;
    for(int i =0;i < int(Field::End_DataBase_Field);i++)
    {
        QString field = meta.valueToKey(i);
        QStringList list = field.split("_");
        QString type = list[list.length()-1];
        if(i != int(Field::End_DataBase_Field)-1)
            fields += field+" " +type+",";
        else
            fields += field+" " +type+")";
    }
    QSqlQuery query(*_conn);
    if(DEBUG)
        Log::d(TAG,"建表sql语句:"+head+fields);
    query.exec(head+fields);
}

QString ImagesTable::table_name()
{
    return  "images554";
}

void ImagesTable::query_this_patient_picture(qint64 patient_id, QList<QJsonObject > & out)
{

    out.clear();
    QString  sql = "select * from "+ table_name() + "  where " + get_field_name(Field::Patient_ID_INTEGER) + "=" +QString::number(patient_id) + " order by " +  get_field_name(Field::Image_ID_TEXT);
    QSqlQuery query(*_conn);
    query.exec(sql);
    if(DEBUG)
    {
        Log::d(TAG,sql);
    }
    while (query.next())
    {

        QJsonObject  obj ;
        QString field;
        QString picture_id;
        field  =  meta.valueToKey(int(Field::Image_ID_TEXT));
        picture_id = query.value(field).toString();
        obj[field] = picture_id;

        field   =  meta.valueToKey(int(Field::Patient_ID_INTEGER));
        obj[field] = query.value(field).toLongLong();

        field   =  meta.valueToKey(int(Field::Image_Path_TEXT));
        obj[field] =Tools::get_root_path()+"/"+  query.value(field).toString();

        field   =  meta.valueToKey(int(Field::Image_Last_State_Path_TEXT));
        obj[field] = Tools::get_root_path()+"/"+ query.value(field).toString();


        field   =  meta.valueToKey(int(Field::Image_Pillar_Path_TEXT));
        obj[field] = Tools::get_root_path()+"/"+ query.value(field).toString();

        field   =  meta.valueToKey(int(Field::Describe_TEXT));
        obj[field] = query.value(field).toString();

        field   =  meta.valueToKey(int(Field::Tee_TEXT));
        obj[field] = query.value(field).toString();

       // field   =  meta.valueToKey(int(Field::Tee_from));
       // obj[field] = query.value(field).toString();

        field   =  meta.valueToKey(int(Field::Tee_Type_TEXT));
        obj[field] = query.value(field).toString();

        field   =  meta.valueToKey(int(Field::Tee_Indexs_TEXT));
        obj[field] = query.value(field).toString();

        field   =  meta.valueToKey(int(Field::Window_Begin_integer));
        obj[field] = query.value(field).toInt();

        field   =  meta.valueToKey(int(Field::Window_End_integer));
        obj[field] = query.value(field).toInt();

        field   =  meta.valueToKey(int(Field::Level_integer));
        obj[field] = query.value(field).toInt();

        field   =  meta.valueToKey(int(Field::Source_Type_integer));
        obj[field] = query.value(field).toInt();

        field   =  meta.valueToKey(int(Field::Scale_float));
        obj[field] = query.value(field).toFloat();

        field   =  meta.valueToKey(int(Field::Rotate_integer));
        obj[field] = query.value(field).toFloat();

        field   =  meta.valueToKey(int(Field::Luminance_float));
        obj[field] = query.value(field).toFloat();

        field   =  meta.valueToKey(int(Field::Contrast_float));
        obj[field] = query.value(field).toFloat();

        field   =  meta.valueToKey(int(Field::Gama_float));
        obj[field] = query.value(field).toFloat();

        field   =  meta.valueToKey(int(Field::Float_float));
        obj[field] = query.value(field).toFloat();

        field   =  meta.valueToKey(int(Field::Rui_Hua_integer));
        obj[field] = query.value(field).toFloat();


        field   =  meta.valueToKey(int(Field::Enable_Fake_Color_Filter_BOOLEAN));
        obj[field] = query.value(field).toBool();
        field   =  meta.valueToKey(int(Field::Enable_Turn_Color_Filter_BOOLEAN));
        obj[field] = query.value(field).toBool();

//        field   =  meta.valueToKey(int(Field::Enable_High_USM__Filter_BOOLEAN));
//        obj[field] = query.value(field).toBool();
//        field   =  meta.valueToKey(int(Field::Enable_High_DH__Filter_BOOLEAN));
//        obj[field] = query.value(field).toBool();
//        field   =  meta.valueToKey(int(Field::Enable_Mouse_Detail__Filter_BOOLEAN));
//        obj[field] = query.value(field).toBool();
        field   =  meta.valueToKey(int(Field::Filter_Index_integer));
        obj[field] = query.value(field).toInt();


        field   =  meta.valueToKey(int(Field::Enable_Hor_Mirror_BOOLEAN));
        obj[field] = query.value(field).toBool();
        field   =  meta.valueToKey(int(Field::Enable_Ver_Mirror_BOOLEAN));
        obj[field] = query.value(field).toBool();




        field   =  meta.valueToKey(int(Field::Width_integer));
        obj[field] = query.value(field).toInt();

        field   =  meta.valueToKey(int(Field::Height_integer));
        obj[field] = query.value(field).toInt();


        field   =  meta.valueToKey(int(Field::Image_State_Integer));
        obj[field] = query.value(field).toInt();

        field   =  meta.valueToKey(int(Field::Pixel_Space_Hor_MM_Float));
        obj[field] = query.value(field).toFloat();

        field   =  meta.valueToKey(int(Field::Pixel_Space_Ver_MM_Float));
        obj[field] = query.value(field).toFloat();

        field   =  meta.valueToKey(int(Field::Expose_Index_integer));
        obj[field] = query.value(field).toInt();

        field   =  meta.valueToKey(int(Field::Expose_Value_TEXT));
        obj[field] = query.value(field).toString();

        field   =  meta.valueToKey(int(Field::Expose_Cm_TEXT));
        obj[field] = query.value(field).toString();

        field   =  meta.valueToKey(int(Field::Voltage_V_TEXT));
        obj[field] = query.value(field).toString();

        field   =  meta.valueToKey(int(Field::Electricity_A_TEXT));
        obj[field] = query.value(field).toString();
        //不属于数据库的用户操作状态字段

//        field   =  meta.valueToKey(int(Field::Current_Line_Task_integer));
//        obj[field] =  int(LineTask::NoLineTask);

        field   =  meta.valueToKey(int(Field::Enable_Draw_Rule_Bool));
        obj[field] = false;


        field   =  meta.valueToKey(int(Field::Checked_For_Export_Or_Delete_Bool));
        obj[field] = false;

        field   =  meta.valueToKey(int(Field::Checked_For_Add_To_Paint_List_Bool));
        obj[field] = false;

        field   =  meta.valueToKey(int(Field::Checked_For_Painting_Bool));
        obj[field] = false;

        field   =  meta.valueToKey(int(Field::Old_Value_JsonArray_Str));
        obj[field] = "[]";

        field   =  meta.valueToKey(int(Field::Lines_JsonArray_Str));
        obj[field] = "[]";

        field   =  meta.valueToKey(int(Field::Url_str));
        obj[field] = "image://editing_images/"+ picture_id+ "/-1";


        QDateTime time = QDateTime::fromMSecsSinceEpoch(picture_id.toLongLong()); //时间戳-毫秒级
        QString name = time.toString("yyyy-MM-dd hh:mm:ss");
        field   =  meta.valueToKey(int(Field::File_Name_Str));
        obj[field] = name ;
        out.append(obj);
    }
}

void ImagesTable::query_picture(qint64 patient_id,const QString &picture_id, QJsonObject &out)
{
    QString  sql = "select * from "+ table_name() + "  where " + get_field_name(Field::Patient_ID_INTEGER) + "=" +QString::number(patient_id) + " and  " +get_field_name(Field::Image_ID_TEXT) + "='" + picture_id+"'";
    QSqlQuery query(*_conn);
    query.exec(sql);
    if(DEBUG)
    {
        Log::d(TAG,sql);
    }
    while (query.next())
    {

        QJsonObject & obj = out;
        QString field;
        QString picture_id;
        field  =  meta.valueToKey(int(Field::Image_ID_TEXT));
        picture_id = query.value(field).toString();
        obj[field] = picture_id;

        field   =  meta.valueToKey(int(Field::Patient_ID_INTEGER));
        obj[field] = query.value(field).toLongLong();

        field   =  meta.valueToKey(int(Field::Image_Path_TEXT));
        obj[field] = Tools::get_root_path()+"/"+ query.value(field).toString();

        field   =  meta.valueToKey(int(Field::Image_Last_State_Path_TEXT));
        obj[field] =Tools::get_root_path()+"/"+  query.value(field).toString();


        field   =  meta.valueToKey(int(Field::Image_Pillar_Path_TEXT));
        obj[field] =Tools::get_root_path()+"/"+  query.value(field).toString();

        field   =  meta.valueToKey(int(Field::Describe_TEXT));
        obj[field] = query.value(field).toString();

        field   =  meta.valueToKey(int(Field::Tee_Type_TEXT));
        obj[field] = query.value(field).toString();

       // field   =  meta.valueToKey(int(Field::Tee_from));
       // obj[field] = query.value(field).toString();

        field   =  meta.valueToKey(int(Field::Tee_Indexs_TEXT));
        obj[field] = query.value(field).toString();

        field   =  meta.valueToKey(int(Field::Tee_TEXT));
        obj[field] = query.value(field).toString();

        field   =  meta.valueToKey(int(Field::Window_Begin_integer));
        obj[field] = query.value(field).toInt();

        field   =  meta.valueToKey(int(Field::Window_End_integer));
        obj[field] = query.value(field).toInt();

        field   =  meta.valueToKey(int(Field::Level_integer));
        obj[field] = query.value(field).toInt();

        field   =  meta.valueToKey(int(Field::Source_Type_integer));
        obj[field] = query.value(field).toInt();

        field   =  meta.valueToKey(int(Field::Scale_float));
        obj[field] = query.value(field).toFloat();

        field   =  meta.valueToKey(int(Field::Rotate_integer));
        obj[field] = query.value(field).toFloat();

        field   =  meta.valueToKey(int(Field::Luminance_float));
        obj[field] = query.value(field).toFloat();

        field   =  meta.valueToKey(int(Field::Contrast_float));
        obj[field] = query.value(field).toFloat();

        field   =  meta.valueToKey(int(Field::Gama_float));
        obj[field] = query.value(field).toFloat();

        field   =  meta.valueToKey(int(Field::Float_float));
        obj[field] = query.value(field).toFloat();

        field   =  meta.valueToKey(int(Field::Rui_Hua_integer));
        obj[field] = query.value(field).toFloat();


        field   =  meta.valueToKey(int(Field::Enable_Fake_Color_Filter_BOOLEAN));
        obj[field] = query.value(field).toBool();
        field   =  meta.valueToKey(int(Field::Enable_Turn_Color_Filter_BOOLEAN));
        obj[field] = query.value(field).toBool();

//        field   =  meta.valueToKey(int(Field::Enable_High_USM__Filter_BOOLEAN));
//        obj[field] = query.value(field).toBool();
//        field   =  meta.valueToKey(int(Field::Enable_High_DH__Filter_BOOLEAN));
//        obj[field] = query.value(field).toBool();
//        field   =  meta.valueToKey(int(Field::Enable_Mouse_Detail__Filter_BOOLEAN));
//        obj[field] = query.value(field).toBool();
        field   =  meta.valueToKey(int(Field::Filter_Index_integer));
        obj[field] = query.value(field).toInt();


        field   =  meta.valueToKey(int(Field::Enable_Hor_Mirror_BOOLEAN));
        obj[field] = query.value(field).toBool();
        field   =  meta.valueToKey(int(Field::Enable_Ver_Mirror_BOOLEAN));
        obj[field] = query.value(field).toBool();




        field   =  meta.valueToKey(int(Field::Width_integer));
        obj[field] = query.value(field).toInt();

        field   =  meta.valueToKey(int(Field::Height_integer));
        obj[field] = query.value(field).toInt();


        field   =  meta.valueToKey(int(Field::Image_State_Integer));
        obj[field] = query.value(field).toInt();

        field   =  meta.valueToKey(int(Field::Pixel_Space_Hor_MM_Float));
        obj[field] = query.value(field).toFloat();

        field   =  meta.valueToKey(int(Field::Pixel_Space_Ver_MM_Float));
        obj[field] = query.value(field).toFloat();

        field   =  meta.valueToKey(int(Field::Expose_Index_integer));
        obj[field] = query.value(field).toInt();


        field   =  meta.valueToKey(int(Field::Expose_Value_TEXT));
        obj[field] = query.value(field).toString();

        field   =  meta.valueToKey(int(Field::Expose_Cm_TEXT));
        obj[field] = query.value(field).toString();

        field   =  meta.valueToKey(int(Field::Voltage_V_TEXT));
        obj[field] = query.value(field).toString();

        field   =  meta.valueToKey(int(Field::Electricity_A_TEXT));
        obj[field] = query.value(field).toString();
        //不属于数据库的用户操作状态字段

//        field   =  meta.valueToKey(int(Field::Current_Line_Task_integer));
//        obj[field] =  int(LineTask::NoLineTask);

        field   =  meta.valueToKey(int(Field::Enable_Draw_Rule_Bool));
        obj[field] = false;


        field   =  meta.valueToKey(int(Field::Checked_For_Export_Or_Delete_Bool));
        obj[field] = false;

        field   =  meta.valueToKey(int(Field::Checked_For_Add_To_Paint_List_Bool));
        obj[field] = false;

        field   =  meta.valueToKey(int(Field::Checked_For_Painting_Bool));
        obj[field] = false;

        field   =  meta.valueToKey(int(Field::Old_Value_JsonArray_Str));
        obj[field] = "[]";

        field   =  meta.valueToKey(int(Field::Lines_JsonArray_Str));
        obj[field] = "[]";

        field   =  meta.valueToKey(int(Field::Url_str));
        obj[field] = "image://editing_images/"+ picture_id+ "/-1";


        QDateTime time = QDateTime::fromMSecsSinceEpoch(picture_id.toLongLong()); //时间戳-毫秒级
        QString name = time.toString("yyyy-MM-dd hh:mm:ss");
        field   =  meta.valueToKey(int(Field::File_Name_Str));
        obj[field] = name ;
    }
}

QString ImagesTable::query_picture_path(QString picture_id)
{
    QString  sql = "SELECT Image_Path_TEXT FROM images WHERE Image_ID_TEXT = :Image_ID_TEXT";
    QSqlQuery query(*_conn);
    query.prepare(sql);
    query.bindValue(":Image_ID_TEXT", picture_id);
    if(DEBUG)
    {
        Log::d(TAG,sql);
    }
    if (query.exec()) {
        while (query.next()) {
            return query.value("Image_Path_TEXT").toString();
        }
    }
    return "";
}

bool ImagesTable::updateImageNote(QString picture_id, QString image_note)
{
    QString  sql = "UPDATE images SET Describe_TEXT = :Describe_TEXT WHERE Image_ID_TEXT = :Image_ID_TEXT";
    QSqlQuery query(*_conn);
    query.prepare(sql);
    query.bindValue(":Describe_TEXT", image_note);
    query.bindValue(":Image_ID_TEXT", picture_id);
    if(DEBUG)
    {
        Log::d(TAG,sql);
    }
    if (query.exec()) {
        return true;
    }
    return false;
}

bool ImagesTable::moveImage(QString picture_id, qint64 patient_id)
{
    QString  sql = "UPDATE images SET Patient_ID_INTEGER = :Patient_ID_INTEGER WHERE Image_ID_TEXT = :Image_ID_TEXT";
    QSqlQuery query(*_conn);
    query.prepare(sql);
    query.bindValue(":Patient_ID_INTEGER", patient_id);
    query.bindValue(":Image_ID_TEXT", picture_id);
    if(DEBUG)
    {
        Log::d(TAG,sql);
    }
    if (query.exec()) {
        return true;
    }
    return false;
}
