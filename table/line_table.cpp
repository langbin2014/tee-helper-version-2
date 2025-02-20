#include "line_table.h"
#include<QSqlError>
//#include "global.h"
#include"c++/log.h"
LineTable * LineTable::_single_instance = nullptr;
LineTable::LineTable(QObject *parent):QObject(parent),meta(QMetaEnum::fromType<LineTable::Field>()),DataBase()
{
    check_table_or_create_it();
}

LineTable *LineTable::getInstance()
{
    if(_single_instance == nullptr)
        _single_instance = new LineTable();
    return  _single_instance;
}

QString LineTable::get_field_name(LineTable::Field field)
{
    return meta.valueToKey(int(field));
}

int LineTable::get_field_value(const QString &field_str)
{
    return meta.keyToValue(field_str.toLatin1().data());
}

bool LineTable::add(const LineTypesNS::Line &line)
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
    QString pts_str = line.image_id;
    query.bindValue(":"+field,pts_str);

    field   =  meta.valueToKey(int(Field::Patient_ID_INTEGER));
    query.bindValue(":"+field,line.patient_id);

    field   =  meta.valueToKey(int(Field::Line_ID_INTEGER));
    query.bindValue(":"+field,line.line_id);

    field   =  meta.valueToKey(int(Field::Line_Color_TEXT));
    query.bindValue(":"+field,line.line_color);

    field   =  meta.valueToKey(int(Field::Line_Type_integer));
    query.bindValue(":"+field,int(line.line_type));

    field   =  meta.valueToKey(int(Field::Line_Data_BLOB));
    QByteArray line_data((const char *)line.line_data.data(),line.line_data.size() * sizeof (LineTypesNS::Point));
    query.bindValue(":"+field,line_data,QSql::Binary);

    bool ok =  query.exec();
    if(!ok)
        Log::e(TAG,"新增线条保存失败:"+query.lastError().text());
    return ok;
}

void LineTable::update_color(qint64 id, const QString &color)
{
    if(DEBUG)
        Log::d(TAG,"正在更新线颜色:"+color);
    QSqlQuery q(*_conn);
    QString key = get_field_name(Field::Image_ID_TEXT);
    QString color_field   =  meta.valueToKey(int(Field::Line_Color_TEXT));
    QString line_field = meta.valueToKey(int(Field::Line_ID_INTEGER));
    QString template_str = "update  "+ table_name() + " set "+color_field +" = '" +color+ "' where " + line_field + " = " + QString::number(id);
    bool ok = q.exec(template_str);
    if(!ok)
        Log::e(TAG,"更新线颜色错误:"+q.lastError().text());
}

void LineTable::update_line_data(qint64 id, const std::vector<LineTypesNS::Point> &line_data)
{
    QMutexLocker lock(_lock);
    if(DEBUG)
        Log::d(TAG,"正在更新线");

    QString data_field = meta.valueToKey(int(Field::Line_Data_BLOB));
    QString line_id_field = meta.valueToKey(int(Field::Line_ID_INTEGER));

    QSqlQuery query(*_conn);
    query.prepare("UPDATE  "+ table_name() + " SET "+data_field+ " = :"+data_field+" where " + line_id_field + " = :" + line_id_field);
    QByteArray line_data_((const char *)line_data.data(),line_data.size() * sizeof (LineTypesNS::Point));
    query.bindValue(":"+data_field,line_data_,QSql::Binary);
    query.bindValue(":"+line_id_field,id);
    bool ok = query.exec();
    if(!ok)
        Log::e(TAG,"更新线条数据错误:"+query.lastError().text());
}

void LineTable::remove(qint64 line_id)
{
    if(DEBUG)
        Log::d(TAG,"正在删除数据库线条:"+QString::number(line_id));
    QSqlQuery q(*_conn);
    QString key = get_field_name(Field::Line_ID_INTEGER);
    QString template_str = "delete  from  "+ table_name() + " where " + key + " = " + QString::number(line_id);
    bool ok =  q.exec(template_str);
    if(!ok)
        Log::e(TAG,"删除线条错误:"+q.lastError().text());
}

void LineTable::remove_this_picture_all_lines(const QString &picture_id)
{
    if(DEBUG)
        Log::d(TAG,"正在图片的所有数据库线条:"+picture_id);
    QSqlQuery q(*_conn);
    QString key = get_field_name(Field::Image_ID_TEXT);
    QString template_str = "delete  from  "+ table_name() + " where " + key + " = " + picture_id;
    bool ok =  q.exec(template_str);
    if(!ok)
        Log::e(TAG,"删除图片的所有线条错误:"+q.lastError().text());
}

void LineTable::remove_this_patient_all_lines(qint64 patient_id)
{
    QString id_str = QString::number(patient_id);
    if(DEBUG)
        Log::d(TAG,"正在删除病人所有线条:"+id_str);
    QSqlQuery q(*_conn);
    QString key = get_field_name(Field::Patient_ID_INTEGER);
    QString template_str = "delete  from  "+ table_name() + " where " + key + " = " +id_str;
    bool ok =  q.exec(template_str);
    if(!ok)
        Log::e(TAG,"删除病人所有线条:"+q.lastError().text());
}

void LineTable::query_this_patient_lines(qint64 patient_id, std::map<QString,std::shared_ptr<std::list<std::shared_ptr<LineTypesNS::Line>>>> &out)
{
    out.clear();
    QString  sql = "select * from "+ table_name() + "  where " + get_field_name(Field::Patient_ID_INTEGER) + "=" +QString::number(patient_id) + " order by " +  get_field_name(Field::Line_ID_INTEGER);
    QSqlQuery query(*_conn);
    query.exec(sql);
    if(DEBUG)
    {
        Log::d(TAG,sql);
    }
    while (query.next())
    {

        std::shared_ptr<LineTypesNS::Line> line(new LineTypesNS::Line) ;
        QString field;
        field  =  meta.valueToKey(int(Field::Image_ID_TEXT));
        line->image_id = query.value(field).toString();

        field  =  meta.valueToKey(int(Field::Patient_ID_INTEGER));
        line->patient_id = qint64(query.value(field).toDouble());

        field  =  meta.valueToKey(int(Field::Line_ID_INTEGER));
        line->line_id = qint64(query.value(field).toDouble());

        field  =  meta.valueToKey(int(Field::Line_Color_TEXT));
        line->line_color = query.value(field).toString();

        field  =  meta.valueToKey(int(Field::Line_Type_integer));
        line->line_type = LineTypesNS::Type(query.value(field).toInt());

        field  =  meta.valueToKey(int(Field::Line_Data_BLOB));
        QByteArray line_data = query.value(field).toByteArray();
        line->line_data.resize(line_data.size()/ sizeof (LineTypesNS::Point));
        memcpy(line->line_data.data(),line_data.data(),line_data.size());
        query.bindValue(":"+field,line_data,QSql::Binary);

        std::shared_ptr<std::list<std::shared_ptr<LineTypesNS::Line>>> lines;

        auto it = out.find(line->image_id);
        if(it == out.end())
        {
            lines =   std::shared_ptr<std::list<std::shared_ptr<LineTypesNS::Line>>>(new std::list<std::shared_ptr<LineTypesNS::Line>>());
            out[line->image_id] =lines;
        }
        else
            lines = it->second;
        lines->push_back(line);
    }
}


void LineTable::on_creating_table()
{
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

QString LineTable::table_name()
{
    return "lines";
}




