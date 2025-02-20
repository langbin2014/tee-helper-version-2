#ifndef PATIENT_MODEL_H
#define PATIENT_MODEL_H
#include <QJsonArray>
#include <QJsonObject>
#include <QJSValue>
#include <QAbstractListModel>

#include<QMetaEnum>
#include"c++/define_tool.h"
#include"c++/settings.h"
class Patient_Model :public QAbstractListModel
{
    const QString TAG = "基础设置类";
    Q_OBJECT
public:
    enum class Fields{ //添加字段都要在 query函数里面添加对应的字段转换
        address_TEXT,
        avatar_TEXT, //头像
        birthday_TEXT,
      //  birthday_INTEGER,
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


    Q_ENUM(Fields)
    Patient_Model(QObject * parent = nullptr);
    QString get_field_name(Fields field) const//枚举转字符串
    {
        return meta.valueToKey(int(field));
    }

    int get_field_value(const QString &field_str) const//字符串转枚举
    {
        return meta.keyToValue(field_str.toLatin1().data());
    }

    void reset(const QJsonArray &all_info); //重置 数据
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;//获得数据
    bool setData(const QModelIndex &index, const QVariant &value, int role ) override;//设置数据
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;//行数


    QJsonObject update(qint64 id, const QJsonObject &info);
    bool append(const QJsonObject &info);
    QJsonObject remove(qint64 id);

    void clear();
    QJsonObject get_info(qint64 id);

protected:
    virtual QHash<int, QByteArray> roleNames() const override; //角色名
    QList<QJsonObject> all_info;
    QHash<int, QByteArray> roles;
    QMetaEnum meta;

};




#endif // PATIENT_MODEL_H
