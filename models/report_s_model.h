#ifndef REPORT_S_MODEL_H
#define REPORT_S_MODEL_H

#include <QJsonArray>
#include <QJsonObject>
#include <QJSValue>
#include <QAbstractListModel>

#include<QMetaEnum>
#include"c++/define_tool.h"
#include"c++/settings.h"
class Report_s_Model :public QAbstractListModel
{
    const QString TAG = "基础设置类";
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


    Q_ENUM(Fields)
    Report_s_Model(QObject * parent = nullptr);

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
    QJsonArray get_all_info();

protected:
    virtual QHash<int, QByteArray> roleNames() const override; //角色名
    QList<QJsonObject> all_info;
    QHash<int, QByteArray> roles;
    QMetaEnum meta;

};




#endif // REPORT_S_MODEL_H
