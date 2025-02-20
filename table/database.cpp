#include"database.h"
#include<QMetaEnum>
#include<QDebug>
#include"c++/tools.h"
#include<QSqlError>
QSqlDatabase * DataBase::_conn = new QSqlDatabase();
QMutex  *  DataBase::_lock = new QMutex();
bool DataBase::inited = false;
DataBase::DataBase(){
}

void DataBase::init(const QString & db_file)
{
    Tools::make_path(db_file,1);

    if(_conn->isOpen())
    {
        _conn->close();
    }

    *_conn = QSqlDatabase::addDatabase("QSQLITE");
    _conn->setDatabaseName(db_file);
    inited = _conn->open();
    qDebug() << "备份数据库链接" << db_file << inited;
}



void DataBase::check_table_or_create_it()
{
   // QMutexLocker l(_lock);

    QString sql = QString("select * from sqlite_master where name='%1'").arg(table_name());
    QSqlQuery query(*_conn);
    query.exec(sql);
    bool inited = query.next();
    if(inited == false)
    {
        qDebug() << "没有有表:"+table_name()+",创建它----------";
        on_creating_table();
        QString sql = QString("select * from sqlite_master where name='%1'").arg(table_name());
        QSqlQuery query(*_conn);
        query.exec(sql);
        inited = query.next();
        if(inited == false)
            qDebug() << "创建表"<< table_name() << "失败"  << query.lastError() <<"--------"<<query.boundValues() <<*_conn;
        else
                qDebug() << "创建表"<< table_name() << "成功";
    }
    else{
       qDebug() << "已经有表:"+table_name()+",不再创建它";
    }
}

//QJsonArray DataBase::query(const QString & condition,const QStringList & neep_field,const QString & order)
//{ //https://www.jb51.cc/sqlite/198774.html
//    return QJsonArray();
//}

