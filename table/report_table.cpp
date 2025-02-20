#include "report_table.h"

#include <QSqlError>
#include<QDebug>
#include<QDate>
#include<QDateTime>
#include"../tools.h"
#include"../settings.h"
Report_Table * Report_Table::_single_instance = nullptr;

Report_Table::Report_Table(QObject *parent):QObject(parent),meta(QMetaEnum::fromType<Report_Table::Fields>()),DataBase()
{
    check_table_or_create_it();
}

Report_Table *Report_Table::get_instance()
{
    if(_single_instance == nullptr)
        _single_instance = new Report_Table;
    return  _single_instance;
}


bool Report_Table::add_report(const QJsonObject & info)
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
        query.bindValue(":"+key,info.value(key).toVariant());
    }


    bool ok =  query.exec();
    if(!ok)
        qDebug() <<  "新增记录失败:"+query.lastError().text();
    return ok;
}





QJsonArray Report_Table::query(const QString &if_str)
{
    QJsonArray out;
    QString  sql ;
    if(if_str.isNull())
        sql = "select * from "+ table_name();
    else
        sql = "select * from "+ table_name() + "  where "+if_str +" ORDER BY report_id_pts_INTEGER";// , pts_INTEGER";
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

QJsonArray Report_Table::query_all()
{
    return query(QString());
}

QJsonObject Report_Table::create_empty_report(qint64 patient_pts_id, const QString &doctor,const QString &current_patient)
{
    QDateTime today = QDateTime::currentDateTime();
    int count =  get_patient_report_count(patient_pts_id)+1;
    QJsonObject report;
    QString count_str = QString::number(count);

    while (count_str.size() <4) {
        count_str = "0"+count_str;
    }

    report["doctor_TEXT"] = doctor;
    report["patient_id_INTEGER"] = patient_pts_id;

    qint64 id = today.toSecsSinceEpoch();
    report["report_id_pts_INTEGER"] = id;

    report["mark_TEXT"] = "";

    QJsonArray content;
    QJsonObject ele;
    ele["image_url"]="";
    ele["performance_string"]="";
    ele["conclusion_string"] ="";

    content.append(ele);
    QString str;
    Tools::jsonObj2String(content,str);

   // report["report_content_json_array_TEXT"] =str;
    report["report_pic_url_TEXT"] = QString::number(id)+".png";


    auto st = Settings::get_instance();

    QString name2 = st->tran("报告");
    QString name3 = QString::number(count);
    if(count <10)
        name3 = "0"+name3;
    report["report_name_TEXT"] = current_patient+"_"+name2+"_"+name3;
    report["report_NO_TEXT"] =  "NO."+ today.toString("yyyy/MM/dd-")+name3;

   // add_report(report);
    return report;
}





void Report_Table::remove(const QString &if_str)
{
    QSqlQuery q(*_conn);
    QString template_str = "delete  from  "+ table_name() + " where " +if_str;
    if(DEBUG)
        qDebug() <<  "正在删除数据库:"+template_str;
    bool ok =  q.exec(template_str);
    if(!ok)
        qDebug() <<  "删除错误:"+q.lastError().text();
}

void Report_Table::update(const QString &if_str, const QJsonObject &info)
{


    //    QString set_str;
    //    for(auto it = info.begin();it!=info.end();it++)
    //    {
    //        set_str +=(it.key() + "=:"+it.key()) +" ";

    //    }
    //    QString  sql = "UPDATE "+table_name()+" SET "+set_str + "WHERE "+if_str;

    //    QSqlQuery query(*_conn);
    //    query.prepare(sql);

    //    for(auto it = info.begin();it!=info.end();it++)
    //    {
    //        QString key = it.key();
    //        query.bindValue(":"+key, it.value().toVariant());
    //    }

    //    if(DEBUG)
    //    {
    //        qDebug() <<  sql;
    //    }
    //    bool ok = query.exec();
    //    if(!ok)
    //        qDebug() << "更新错误:"+query.lastError().text();


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

int Report_Table::get_patient_report_count(qint64 pts_id_s)
{
    // 执行查询
    QSqlQuery query(*_conn);
    QString sql = "SELECT COUNT(*) FROM "+table_name() + " where  patient_id_INTEGER = "+QString::number(pts_id_s);
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



void Report_Table::on_creating_table()
{
    //   QMutexLocker locker(_lock);
    {
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

}

QString Report_Table::table_name()
{
    return  "report";
}






void Report_Table::reset()
{
    _single_instance->deleteLater();
    _single_instance = nullptr;
    get_instance();
}
