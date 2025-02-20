#include "Patient_table.h"
#include <QSqlError>
#include<QDebug>
#include<QDate>
#include<QDateTime>
#include"../tools.h"
#include"../settings.h"
Patient_Table * Patient_Table::_single_instance = nullptr;
Patient_Table::Patient_Table(QObject *parent):QObject(parent),meta(QMetaEnum::fromType<Patient_Table::Fields>()),DataBase()
{
    check_table_or_create_it();
    connect(this,&Patient_Table::save_info_sig,this,[this](const QJsonObject & info){
        add_patent(info);
    });
}

Patient_Table *Patient_Table::get_instance()
{
    if(_single_instance == nullptr)
        _single_instance = new Patient_Table;
    return  _single_instance;
}


bool Patient_Table::add_patent(const QJsonObject & info)
{
    QMutexLocker lock(_lock);
    QString head = "INSERT INTO " + table_name() +" ";
    QString fields;
    QString fields_alias;
    for(int i =0;i < int(Fields::End_Fields);i++)
    {
        QString field = get_field_name(Fields(i));
        if(i != int(Fields::End_Fields)-1)
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
        qDebug() <<  "生成的追加sql:"+sql;
    QSqlQuery query(*_conn);
    query.prepare(sql);


    QStringList keys = info.keys();
    for(int i =0; i< keys.size();i++)
    {
        const QString & key = keys.at(i);
        if(key.endsWith("INTEGER"))
        {
            query.bindValue(":"+key,int(info.value(key).toDouble()));
        }
        else
            query.bindValue(":"+key,info.value(key).toVariant());
    }


    bool ok =  query.exec();
    if(!ok)
    {
        qDebug() <<  table_name()<< "新增记录失败:"+query.lastError().text();
    }
    return ok;
}





qint64 Patient_Table::query_max_pts(const QString &dev_id)
{
    // 执行查询
    QSqlQuery query(*_conn);
    QString sql = "SELECT MAX(pts_INTEGER) FROM "+table_name() + " where device_id_TEXT = '"+dev_id +"'";
    if (!query.exec(sql)) {
        qDebug() << "查询失败：" << query.lastError().text();
        return 0;
    }
    int maxScore = 0;
    // 处理查询结果
    if (query.next()) {
        maxScore = query.value(0).toInt();
    }

    return  maxScore;
}

int Patient_Table::get_doctor_today_count(const QString &doctor, const QString &date_str)
{
    // 执行查询
    QSqlQuery query(*_conn);
    QString sql = "SELECT COUNT(*) FROM "+table_name() + " where doctor_TEXT = '"+doctor +"' AND watch_date_TEXT = '"+date_str+"'";
    if (!query.exec(sql)) {
        qDebug() << "查询失败：" << query.lastError().text();
        return 0;
    }
    int maxScore = 0;
    // 处理查询结果
    if (query.next()) {
        maxScore = query.value(0).toInt();
    }

    return  maxScore;
}

int Patient_Table::get_doctor_all_patient_count(const QString &doctor)
{
    // 执行查询
    QSqlQuery query(*_conn);
    QString sql = "SELECT COUNT(*) FROM "+table_name() + " where doctor_TEXT = '"+doctor +"'";
    if (!query.exec(sql)) {
        qDebug() << "查询失败：" << query.lastError().text();
        return 0;
    }
    int maxScore = 0;
    // 处理查询结果
    if (query.next()) {
        maxScore = query.value(0).toInt();
    }

    return  maxScore;
}





QJsonArray Patient_Table::query(const QString &if_str)
{
    QJsonArray out;
    QString  sql = "select * from "+ table_name() + "  where "+if_str +" ORDER BY pts_INTEGER  DESC";// , pts_INTEGER";
    QSqlQuery query(*_conn);

    query.exec(sql);

    if(DEBUG)
    {
        qDebug() << sql;
    }
    while (query.next())
    {

        QJsonObject  obj ;

        for(int i =0; i< int(Fields::End_Fields);i++)
        {
            QString f = get_field_name(Fields(i));
            obj[f] = query.value(f).toJsonValue();
        }

        out.append(obj);
    }
    return out;
}



void Patient_Table::remove(const QString &if_str)
{


    QSqlQuery q(*_conn);
    QString template_str = "delete  from  "+ table_name() + " where " +if_str;
    if(DEBUG)
        qDebug() <<  "正在删除数据库:"+template_str;
    bool ok =  q.exec(template_str);
    if(!ok)
        qDebug() <<  "删除错误:"+q.lastError().text();
}

void Patient_Table::update(const QString &if_str, const QJsonObject &info)
{

    QString set_str;
    for(auto it = info.begin();it!=info.end();it++)
    {
        set_str +=(it.key() + "=:"+it.key()) +" ";
        if(it != info.end()-1)
        {
            set_str +=",";
        }

    }
    QString  sql = "UPDATE "+table_name()+" SET "+set_str + "WHERE "+if_str;

    QSqlQuery query(*_conn);
    query.prepare(sql);

    for(auto it = info.begin();it!=info.end();it++)
    {
        query.bindValue(":"+it.key(), it.value().toVariant());
    }

    if(DEBUG)
    {
        qDebug() <<  sql;
    }
    bool ok = query.exec();
    if(!ok)
        qDebug() << "更新错误:"+query.lastError().text();
}

void Patient_Table::remove_this_patient_all_picture(const QString &patient_id)
{

}

QString Patient_Table::get_full_query_str(const QString &and_str, const QString &key_word)
{
    //SQL: "SELECT COUNT(1) AS cnt FROM medical_record  WHERE   ( (patient_id_num LIKE  :patient_id_num1) OR (name LIKE  :name1) OR (gender LIKE  :gender1) OR (ageY LIKE  :ageY1) OR (doctor LIKE  :doctor1) OR (diagnosis_id_num LIKE  :diagnosis_id_num1) OR (birthday LIKE  :birthday1) OR (phone LIKE  :phone1) OR (address LIKE  :address1) OR (complaint_of_illness LIKE  :complaint_of_illness1) OR (doctor_name LIKE  :doctor_name1) OR (account LIKE  :account1))  AND (gender =  :gender0  COLLATE NOCASE )" 1


    QString or_str ;
    if(key_word != "")
    {
        QStringList or_str_list;
        for(int i =0; i< int(Fields::End_Fields);i++)
        {
            QString field = get_field_name(Fields(i));
            if(and_str.contains(field))
                continue;
            else
            {
                if(field.endsWith("TEXT"))
                {
                    or_str_list << (field + " LIKE '%"+key_word+"%'");
                }

                //                else if(field.endsWith("INTEGER"))
                //                {
                //                    or_str_list << ("CAST("+field+" AS VARCHAR)"  + " LIKE '%"+key_word+"%'");
                //                }
            }
        }

        or_str  = "( ";
        for(int i =0; i < or_str_list.size();i++)
        {
            or_str += " ( "  +or_str_list.at(i) +" ) ";
            if(i != or_str_list.size()-1)
            {
                or_str += " OR ";
            }
        }
        or_str += " )";
        return and_str +" AND "+ or_str;
    }
    else
        return and_str;

}

QJsonObject Patient_Table::create_empty_patient(const QString &doctor)
{
    QDateTime today = QDateTime::currentDateTime();
    QString date_str = today.toString("yyyy-MM-dd");
    int count = get_doctor_today_count(doctor,date_str);
    QJsonObject patient;
    QString count_str = QString::number(count);

    while (count_str.size() <4) {
        count_str = "0"+count_str;
    }

    patient["address_TEXT"] = "";
    patient["birthday_TEXT"] = date_str;
 //   patient["birthday_INTEGER"] = today.toSecsSinceEpoch();
    patient["avatar_TEXT"] = "";
    patient["complaint_of_illness_TEXT"] = "";
    patient["id_TEXT"] = date_str+"/"+count_str;
    patient["watch_date_TEXT"] = date_str;
    patient["pts_INTEGER"] =  today.toSecsSinceEpoch(); //秒单位
    patient["diagnosis_id_num_INTEGER"] = get_doctor_all_patient_count(doctor);
    patient["doctor_TEXT"] = doctor;
    patient["sex_INTEGER"] = 0;
    patient["name_TEXT"] = "??";
    patient["phone_TEXT"] = "";
    patient["tee_type_INTEGER"] = 0;//0成人，1小孩
    patient["post_TEXT"] = "";

    return patient;
}

void Patient_Table::on_creating_table()
{
    //   QMutexLocker locker(_lock);
    QString head = QString("CREATE TABLE %1 (").arg(table_name());
    QString fields;
    for(int i =0;i < int(Fields::End_Fields);i++)
    {
        QString field = meta.valueToKey(i);
        QStringList list = field.split("_");
        QString type = list[list.length()-1];
        if(i != int(Fields::End_Fields)-1)
            fields += field+" " +type+",";
        else
            fields += field+" " +type+")";
    }
    QSqlQuery query(*_conn);
    if(DEBUG)
        qDebug() << "建表sql语句:"+head+fields;
    query.exec(head+fields);
}

QString Patient_Table::table_name()
{
    return  "patient";
}






void Patient_Table::reset()
{
    _single_instance->deleteLater();
    _single_instance = nullptr;
    get_instance();
}
