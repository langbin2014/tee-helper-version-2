#include "doctor_model.h"


#include"c++/log.h"
#include"c++/tools.h"
#include<QFile>
Doctor_Model::Doctor_Model(QObject *parent):QAbstractListModel(parent),meta(QMetaEnum::fromType<Doctor_Model::Fields>())
{
    for(int i =0;i < int(Fields::End_Fields);i++ )
    {
        roles[i] = get_field_name(Fields(i)).toLatin1();
    }
}



void Doctor_Model::reset(const QJsonArray &all_infos)
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


QVariant Doctor_Model::data(const QModelIndex &index, int role) const
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



bool Doctor_Model::setData(const QModelIndex &index, const QVariant &value, int role)
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

int Doctor_Model::rowCount(const QModelIndex &parent) const
{
    return all_info.size();
}

QJsonObject Doctor_Model::update(const QString &account, const QJsonObject &info)
{
    QStringList keys = info.keys();
    QString id_key = get_field_name(Fields::account_TEXT);
    QJsonObject ret;
    for(int i =0; i < all_info.size(); i++)
    {
        QJsonObject & ele = all_info[i];
        if(ele.value(id_key).toString()== account)
        {
            for(int k =0; k < keys.size();k++)
            {
                const QString & key = keys.at(k);
                ele[key] = info.value(key);
            }
            emit dataChanged(index(i),index(i));
            ret = ele;
            break;
        }
    }
    return  ret;
}

bool Doctor_Model::append(const QJsonObject &info)
{
    QString id_key = get_field_name(Fields::account_TEXT);
    QString new_id = info.value(id_key).toString();
    for(int i =0; i < all_info.size(); i++)
    {
        QJsonObject & ele = all_info[i];
        if(ele.value(id_key).toString() == new_id)
        {
            return false;
        }
    }
    beginInsertRows(QModelIndex(), 0, 0);
    all_info.insert(0,info);
    endInsertRows();
    return true;
}

QJsonObject Doctor_Model::remove(const QString &account)
{
    QJsonObject temp;
    QString id_key = get_field_name(Fields::account_TEXT);
    for(int i =0; i < all_info.size(); i++)
    {
        QJsonObject & ele = all_info[i];
        if(ele.value(id_key).toString() == account)
        {
            temp = ele;
            beginRemoveRows(QModelIndex(),i,i);
            all_info.removeAt(i);
            endRemoveRows();
            break;
        }
    }
    return temp;
}

void Doctor_Model::clear()
{
    beginResetModel();
    all_info.clear();
    endResetModel();
}

QJsonObject Doctor_Model::get_info(const QString &account)
{
    QJsonObject temp;
    QString id_key = get_field_name(Fields::account_TEXT);
    for(int i =0; i < all_info.size(); i++)
    {
        QJsonObject & ele = all_info[i];
        if(ele.value(id_key).toString()  == account)
        {
            temp = ele;
            break;
        }
    }
    return temp;
}

QJsonArray Doctor_Model::get_all_info()
{
    QJsonArray ret;
    for(int  i=0; i <all_info.size();i++)
    {
        ret.append(all_info.at(i));
    }
    return  ret;
}



QHash<int, QByteArray> Doctor_Model::roleNames() const
{
    return roles;
}

