#ifndef REPORT_TABLE_H
#define REPORT_TABLE_H

#include <QObject>
#include <QMetaEnum>
#include "database.h"
class Report_Table :public QObject,public DataBase
{
    const bool DEBUG = true;
    const QString TAG = "病人表";
    Q_OBJECT

public:

    enum class Fields{ //添加字段都要在 query函数里面添加对应的字段转换
        doctor_TEXT, //医生
        patient_id_INTEGER, //病人id
        report_id_pts_INTEGER, //报告id
        report_name_TEXT, //报告名称
        report_NO_TEXT,//报告编号 //最后图片显示
        mark_TEXT, //备注
        report_pic_url_TEXT, //报告内容
        End_Fields
    };

    Q_ENUM(Fields);

    static Report_Table *get_instance();

    QJsonArray query(const QString &if_str);
    QJsonArray query_all();
    QJsonObject create_empty_report(qint64  patient_pts_id, const QString &doctor, const QString &current_patient);
    void remove(const QString &if_str);
  //  void update(const QString &if_str,const QMap<QString,QVariant> & kvs);
    void update(const QString &if_str,const QJsonObject & info);
    int get_patient_report_count(qint64 pts_id_s);

    QString get_field_name(Fields field) //枚举转字符串
    {
        return meta.valueToKey(int(field));
    }

    int get_field_value(const QString &field_str) const//字符串转枚举
    {
        return meta.keyToValue(field_str.toLatin1().data());
    }
    static void reset();
  bool add_report(const QJsonObject & info);
protected:
    virtual void on_creating_table() override;
    virtual QString table_name()override;

private:

    Report_Table(QObject * parent = nullptr);
    QMetaEnum meta;// = QMetaEnum::fromType<images_table::Field>();
    static Report_Table *  _single_instance;
};




#endif // REPORT_TABLE_H
