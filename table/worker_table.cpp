#include "worker_table.h"

#include <QSqlError>
#include<QDebug>
#include<QDate>
#include<QDateTime>
#include"../tools.h"
#include"../settings.h"
Worker_Table * Worker_Table::_single_instance = nullptr;
Worker_Table::Worker_Table(QObject *parent):QObject(parent),meta(QMetaEnum::fromType<Worker_Table::Fields>()),DataBase()
{
    check_table_or_create_it();
    connect(this,&Worker_Table::save_info_sig,this,[this](const QJsonObject & info){
        add_worker(info);
    });
}

Worker_Table *Worker_Table::get_instance()
{
    if(_single_instance == nullptr)
        _single_instance = new Worker_Table;
    return  _single_instance;
}


bool Worker_Table::add_worker(const QJsonObject & info)
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

qint64 Worker_Table::query_max_pts(const QString &dev_id)
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



QJsonArray Worker_Table::query(const QString &if_str)
{
    QJsonArray out;
    QString  sql ;
    if(if_str.isNull())
        sql = "select * from "+ table_name();
    else
        sql = "select * from "+ table_name() + "  where "+if_str +" ORDER BY id_TEXT";// , pts_INTEGER";
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

QJsonArray Worker_Table::query_all()
{
    return query(QString());
}



void Worker_Table::remove(const QString &if_str)
{


    QSqlQuery q(*_conn);
    QString template_str = "delete  from  "+ table_name() + " where " +if_str;
    if(DEBUG)
        qDebug() <<  "正在删除数据库:"+template_str;
    bool ok =  q.exec(template_str);
    if(!ok)
        qDebug() <<  "删除错误:"+q.lastError().text();
}

void Worker_Table::update(const QString &if_str, const QJsonObject &info)
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



void Worker_Table::on_creating_table()
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
    QJsonObject work;

    //    account_TEXT,
    //    password_TEXT,
    //    name_Text,
    //    id_TEXT, //员工号
    //    sex_INTEGER, //0女，1男，2？
    //    phone_TEXT,
    //    job_INTEGER, //主任,前台,护士长,护士,医生,?
    //    room_TEXT,


    work[get_field_name(Fields::account_TEXT)] = "admin";
    work[get_field_name(Fields::password_TEXT)] = "admin";
    work[get_field_name(Fields::name_TEXT)] = "admin";
    work[get_field_name(Fields::id_TEXT)] = "01";// Tools::get_pts_id();
    work[get_field_name(Fields::sex_INTEGER)] = 1;
    work[get_field_name(Fields::phone_TEXT)] = "";
    work[get_field_name(Fields::job_INTEGER)] = 0;
    work[get_field_name(Fields::room_TEXT)] = "";

    add_worker(work);



}

QString Worker_Table::table_name()
{
    return  "record";
}






void Worker_Table::reset()
{
    _single_instance->deleteLater();
    _single_instance = nullptr;
    get_instance();
}
