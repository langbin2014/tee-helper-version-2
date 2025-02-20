#include "report_s_model.h"

#include"c++/log.h"
#include"c++/tools.h"
#include<QFile>
Report_s_Model::Report_s_Model(QObject *parent):QAbstractListModel(parent),meta(QMetaEnum::fromType<Report_s_Model::Fields>())
{
    for(int i =0;i < int(Fields::End_Fields);i++ )
    {
        roles[i] = get_field_name(Fields(i)).toLatin1();
    }
}



void Report_s_Model::reset(const QJsonArray &all_infos)
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


QVariant Report_s_Model::data(const QModelIndex &index, int role) const
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



bool Report_s_Model::setData(const QModelIndex &index, const QVariant &value, int role)
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

int Report_s_Model::rowCount(const QModelIndex &parent) const
{
    return all_info.size();
}

QJsonObject Report_s_Model::update(qint64 id, const QJsonObject &info)
{
    QStringList keys = info.keys();
    QString id_key = get_field_name(Fields::report_id_pts_INTEGER);
    QJsonObject ret;
    for(int i =0; i < all_info.size(); i++)
    {
        QJsonObject & ele = all_info[i];
        if(qint64(ele.value(id_key).toDouble()) == id)
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

bool Report_s_Model::append(const QJsonObject &info)
{
    QString id_key = get_field_name(Fields::report_id_pts_INTEGER);
    qint64 new_id = info.value(id_key).toDouble();
    for(int i =0; i < all_info.size(); i++)
    {
        QJsonObject & ele = all_info[i];
        if(qint64(ele.value(id_key).toDouble()) == new_id)
        {
            return false;
        }
    }
    int i = all_info.size();
    beginInsertRows(QModelIndex(), i, i);
    all_info.insert(i,info);
    endInsertRows();
    return true;
}

QJsonObject Report_s_Model::remove(qint64 id)
{
    QJsonObject temp;
    QString id_key = get_field_name(Fields::report_id_pts_INTEGER);
    for(int i =0; i < all_info.size(); i++)
    {
        QJsonObject & ele = all_info[i];
        if(qint64(ele.value(id_key).toDouble())  == id)
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

void Report_s_Model::clear()
{
    beginResetModel();
    all_info.clear();
    endResetModel();
}

QJsonObject Report_s_Model::get_info(qint64 id)
{
    QJsonObject temp;
    QString id_key = get_field_name(Fields::report_id_pts_INTEGER);
    for(int i =0; i < all_info.size(); i++)
    {
        QJsonObject & ele = all_info[i];
        if(qint64(ele.value(id_key).toDouble())  == id)
        {
            temp = ele;
            break;
        }
    }
    return temp;
}

QJsonArray Report_s_Model::get_all_info()
{
    QJsonArray ret;
    for(int i =0; i < all_info.size();i++)
    {
        ret.append(all_info.at(i));
    }
    return ret;
}



QHash<int, QByteArray> Report_s_Model::roleNames() const
{
    return roles;
}
