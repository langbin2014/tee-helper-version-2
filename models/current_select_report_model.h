#ifndef CURRENT_SELECT_REPORT_MODEL_H
#define CURRENT_SELECT_REPORT_MODEL_H


#include <QJsonArray>
#include <QJsonObject>
#include <QJSValue>
#include <QAbstractListModel>

#include<QMetaEnum>
#include"c++/define_tool.h"
#include"c++/settings.h"

class Current_Select_Report_Model :public QAbstractListModel
{//数据改变5秒后保存，切换数据立即保存
    const QString TAG = "基础设置类";
    Q_OBJECT
public:
    enum class Fields{ //添加字段都要在 query函数里面添加对应的字段转换
        image_url,
        performance_string,
        conclusion_string,
        End_Fields
    };


    Q_ENUM(Fields)
    Current_Select_Report_Model(QObject * parent = nullptr);
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


    void update(int index, const QJsonObject &info);
    bool append(const QJsonObject &info);
    QJsonObject remove(int index);

    void clear();
    QJsonArray get_all_info();
    void new_page();

protected:
    virtual QHash<int, QByteArray> roleNames() const override; //角色名
    QList<QJsonObject> all_info;
    QHash<int, QByteArray> roles;
    QMetaEnum meta;

};





#endif // CURRENT_SELECT_REPORT_MODEL_H
