#ifndef DOCTOR_MODEL_H
#define DOCTOR_MODEL_H

#include <QJsonArray>
#include <QJsonObject>
#include <QJSValue>
#include <QAbstractListModel>

#include<QMetaEnum>
#include"c++/define_tool.h"
#include"c++/settings.h"

class Doctor_Model :public QAbstractListModel
{
    const QString TAG = "基础设置类";
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


    Q_ENUM(Fields)
    Doctor_Model(QObject * parent = nullptr);
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


    QJsonObject update(const QString & account, const QJsonObject &info);
    bool append(const QJsonObject &info);
    QJsonObject remove(const QString & account);

    void clear();
    QJsonObject get_info(const QString & account);
    QJsonArray get_all_info();

protected:
    virtual QHash<int, QByteArray> roleNames() const override; //角色名
    QList<QJsonObject> all_info;
    QHash<int, QByteArray> roles;
    QMetaEnum meta;

};




#endif // DOCTOR_MODEL_H
