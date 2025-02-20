#ifndef WORKER_TABLE_H
#define WORKER_TABLE_H

#include <QObject>
#include <QMetaEnum>
#include "database.h"
class Worker_Table :public QObject,public DataBase
{
    const bool DEBUG = true;
    const QString TAG = "病人表";
    Q_OBJECT

public:

    enum class Fields{ //添加字段都要在 query函数里面添加对应的字段转换
        account_TEXT,
        password_TEXT,
        name_TEXT,
        id_TEXT, //员工号
        sex_INTEGER, //0女，1男，2？
        phone_TEXT,
        job_INTEGER, //主任,前台,护士长,护士,医生,?
        room_TEXT,
        End_Fields
    };

    Q_ENUM(Fields);
    static Worker_Table *get_instance();
    bool add_worker(const QJsonObject & info);
    qint64 query_max_pts(const QString &dev_id);
    QJsonArray query(const QString &if_str);
    QJsonArray query_all();
    void remove(const QString &if_str);
  //  void update(const QString &if_str,const QMap<QString,QVariant> & kvs);
    void update(const QString &if_str,const QJsonObject & info);


    QString get_field_name(Fields field) //枚举转字符串
    {
        return meta.valueToKey(int(field));
    }

    int get_field_value(const QString &field_str) const//字符串转枚举
    {
        return meta.keyToValue(field_str.toLatin1().data());
    }
    static void reset();
signals:
    void save_info_sig(const QJsonObject & info);
protected:
    virtual void on_creating_table() override;
    virtual QString table_name()override;

private:
    Worker_Table(QObject * parent = nullptr);
    QMetaEnum meta;// = QMetaEnum::fromType<images_table::Field>();
    static Worker_Table *  _single_instance;
};

#endif // WORKER_TABLE_H
