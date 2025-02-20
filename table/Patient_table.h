#ifndef PATIENT_TABLE_H
#define PATIENT_TABLE_H

#include <QObject>
#include <QMetaEnum>
#include "database.h"

class Patient_Table :public QObject,public DataBase
{

    const bool DEBUG = true;

    const QString TAG = "病人表";

    Q_OBJECT

public:

    enum class Fields{ //添加字段都要在 query函数里面添加对应的字段转换
        address_TEXT,
        avatar_TEXT, //头像
        birthday_TEXT,
       // birthday_INTEGER,
        complaint_of_illness_TEXT, //诉求
        id_TEXT,//年月日+表里面的数量+1生产
        watch_date_TEXT, //年月日,用于生成id
        pts_INTEGER, //毫秒时间戳//日期查询,唯一id
        diagnosis_id_num_INTEGER, //第几个病人
        doctor_TEXT,
        sex_INTEGER, //"", "男", "女"
        name_TEXT,
        phone_TEXT,
        tee_type_INTEGER, //0小孩，1成人
        post_TEXT,//邮箱
        End_Fields
    };

    Q_ENUM(Fields);
    static Patient_Table *get_instance();
    bool add_patent(const QJsonObject & info);
    qint64 query_max_pts(const QString &dev_id);
    int get_doctor_today_count(const QString &doctor,const QString & date_str );
    int get_doctor_all_patient_count(const QString &doctor);
    QJsonArray query(const QString &if_str);
    void remove(const QString &if_str);
  //  void update(const QString &if_str,const QMap<QString,QVariant> & kvs);
    void update(const QString &if_str,const QJsonObject & info);

    void remove_this_patient_all_picture(const QString & patient_id);

    QString get_full_query_str(const QString &and_str, const QString &key_word);
    QString get_field_name(Fields field) //枚举转字符串
    {
        return meta.valueToKey(int(field));
    }

    int get_field_value(const QString &field_str) const//字符串转枚举
    {
        return meta.keyToValue(field_str.toLatin1().data());
    }
    QJsonObject create_empty_patient(const QString & doctor);
    static void reset();
signals:
    void save_info_sig(const QJsonObject & info);
protected:
    virtual void on_creating_table() override;
    virtual QString table_name()override;

private:
    Patient_Table(QObject * parent = nullptr);
    QMetaEnum meta;// = QMetaEnum::fromType<images_table::Field>();
    static Patient_Table *  _single_instance;
};

#endif // IMAGES_TABLE_H
