#ifndef DATABASE_H
#define DATABASE_H
#include <QObject>
#include <QString>
#include <QTimer>
#include<QJsonObject>

#include<QThread>
#include<QMutex>
#include<QJsonArray>
#include<QThread>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QMutex>
typedef QSharedPointer<QRecursiveMutex> QRecursiveMutexPtr;
class DataBase
{
    const bool DEBUG = true;
    const QString TAG = "数据基类";
public:
    DataBase();
    static void init(const QString &db_file);
protected:
    virtual void on_creating_table() = 0;//{throw "请实现自己的建表函数";};
    virtual QString table_name()=0;
    void check_table_or_create_it();
protected:
    static  QSqlDatabase * _conn;
    static QMutex  * _lock;
    static bool inited;
};
#endif // DATABASE_H
