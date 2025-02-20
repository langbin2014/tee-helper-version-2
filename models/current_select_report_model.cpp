#include "current_select_report_model.h"

#include"c++/log.h"
#include"c++/tools.h"
#include<QFile>
Current_Select_Report_Model::Current_Select_Report_Model(QObject *parent):QAbstractListModel(parent),meta(QMetaEnum::fromType<Current_Select_Report_Model::Fields>())
{
    for(int i =0;i < int(Fields::End_Fields);i++ )
    {
        roles[i] = get_field_name(Fields(i)).toLatin1();
    }
}

void Current_Select_Report_Model::reset(const QJsonArray &all_infos)
{
    QList<QJsonObject> temp;
    for(int i =0; i <all_infos.size();i++)
    {
        temp.append(all_infos.at(i).toObject());
    }
    beginResetModel();

    this->all_info = temp;
    endResetModel();

}


QVariant Current_Select_Report_Model::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row < 0 || row >= all_info.size()) {
        return QVariant();
    }

    Fields field = Fields(role);
    QString key = get_field_name(field);
    const auto & info = all_info.at(row);
    return info[key].toVariant();

}



bool Current_Select_Report_Model::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int row = index.row();
    if(row < 0 || row >= all_info.size()) {
        return false;
    }
    else
    {
        QJsonObject & ele = all_info[row];
        QString field =get_field_name(Fields(role));
        QJsonValue old_value = ele[field];
        QJsonValue new_value  = value.toJsonValue();
        if(old_value == new_value)
            return false;
        else
        {
            ele[field] =new_value ;
            emit dataChanged(index, index);
            return true;
        }
    }
}

int Current_Select_Report_Model::rowCount(const QModelIndex &parent) const
{
    return all_info.size();
}

void Current_Select_Report_Model::update(int index, const QJsonObject &info)
{
    QStringList keys = info.keys();

    QJsonObject & ele = all_info[index];

    for(int k =0; k < keys.size();k++)
    {
        const QString & key = keys.at(k);
        ele[key] = info.value(key);
    }

    emit dataChanged(this->index(index),this->index(index));



}

bool Current_Select_Report_Model::append(const QJsonObject &info)
{

    beginInsertRows(QModelIndex(), 0, 0);
    all_info.insert(0,info);
    endInsertRows();
    return true;
}

QJsonObject Current_Select_Report_Model::remove(int index)
{
    QJsonObject ret;
    beginRemoveRows(QModelIndex(), index, index);
    ret = all_info.at(index);
    all_info.removeAt(index);
    endRemoveRows();
    return ret;
}

void Current_Select_Report_Model::clear()
{
    beginResetModel();
    all_info.clear();
    endResetModel();
}

QJsonArray Current_Select_Report_Model::get_all_info()
{
    QJsonArray ret;

    for(int i =0; i < all_info.size(); i++)
    {
        ret.append(all_info.at(i));
    }
    return  ret;
}

void Current_Select_Report_Model::new_page()
{
    QJsonObject info;

    info["image_url"] = "";
    info["performance_string"] = "";
    info["conclusion_string"] = "";

    int i = all_info.size();
    beginInsertRows(QModelIndex(), i, i);
    all_info.insert(i,info);
    endInsertRows();
}





QHash<int, QByteArray> Current_Select_Report_Model::roleNames() const
{
    return roles;
}
