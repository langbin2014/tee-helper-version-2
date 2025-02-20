#include "eidt_page_editing_picture_model.h"
#include "c++/tools.h"
#include <QFile>
#include"c++/log.h"

EditingPictureModel::EditingPictureModel(QObject *parent):QAbstractListModel(parent)
{
    _today_pts = Tools::get_1970_ms();
    for(int i =0;i < int(ImagesTable::Field::End_Field);i++ )
    {
        if(i == int(ImagesTable::Field::End_DataBase_Field))
            continue;
        roles[i] = ImagesTable::getInstance()->get_field_name(ImagesTable::Field(i)).toLatin1();
    }
    null_info = ImagesTable::getInstance()->create_empty_image_info();
}

void EditingPictureModel::reset_to_empty(int grid_index)
{
 const  int grid_child_s[5] = {1*1,1*2,2*2,2*4,2*6};

    int eles_count = grid_child_s[grid_index];

    QList<QJsonObject> infos;
    QString painting_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Checked_For_Painting_Bool);
    for(int i =0;i < eles_count;i++)
    {
        QJsonObject temp = null_info;
        if(i == 0)
            temp[painting_key] = true;
        else
            temp[painting_key] = false;
        infos.push_back(temp);
    }

    beginResetModel();
    _selected_data = infos;
    endResetModel();

    qDebug() << _selected_data.size() <<eles_count <<"-------333--";

}

void EditingPictureModel::clear()
{
    beginResetModel();
    _selected_data.clear();
    endResetModel();
    setCount(0);
}


void EditingPictureModel::checked_export_or_delete(bool checked)
{
    for(int i = 0; i < _selected_data.size();i++)
    {
        setData(index(i),QVariant(checked),int(ImagesTable::Field::Checked_For_Export_Or_Delete_Bool));
    }
}


void EditingPictureModel::append(const QJsonObject &info)
{
    QString painting_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Checked_For_Painting_Bool);
    QString key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT);
    QString scale_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Scale_float);

    QString new_picture_id = info[key].toString();

    for(int i =0 ;i < _selected_data.size();i ++)
    {
        QJsonObject  ele = _selected_data.at(i);
        QString this_ele_id = ele[key].toString();
        if(this_ele_id == new_picture_id)
        {
            Log::i(TAG,"图片id已经存在:放弃append:"+new_picture_id);
            return ;
        }
    }

    for(int i = 0;i < _selected_data.size();i++)
    {
        QJsonObject & ele = _selected_data[i];

        if(ele[painting_key].toBool() == true)
        {
            ele = info;
            ele[painting_key] = true;

            dataChanged(index(i),index(i));
            break;
        }
    }

//    QJsonObject temp = info;
//    beginInsertRows(QModelIndex(),_selected_data.size(), _selected_data.size());
//    temp[painting_key] = true;
//    temp[scale_key] = 0.25;
//    _selected_data.append(temp);
//    endInsertRows();
//    setCount(_selected_data.size());

    qDebug() << _selected_data.size();

    return ;
}

void EditingPictureModel::remove(const QString &picture_id)
{
    QString key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT);
    QString painting_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Checked_For_Painting_Bool);


    for(int i =0 ;i < _selected_data.size();i ++)
    {
        QJsonObject & ele = _selected_data[i];
        if(ele[key].toString() == picture_id)
        {
            QJsonObject temp = null_info;
            temp[painting_key] = ele[painting_key];
            ele = temp;
            dataChanged(index(i),index(i));
//            beginRemoveRows(QModelIndex(), i, i);
//            _selected_data.removeAt(i);
//            endRemoveRows();
            break;
        }
    }
//    setCount(_selected_data.size());
//    QString painting_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Checked_For_Painting_Bool);
//    int role = roles.key(painting_key.toLatin1());
//    for(int i = 0;i < _selected_data.size();i++)
//    {
//        if(i == _selected_data.size()-1)
//        {
//            setData(index(i),QVariant(true),role);
//        }
//        else
//        {
//            const QJsonObject & ele = _selected_data.at(i);
//            if(ele[painting_key].toBool() == true)
//            {
//                setData(index(i),QVariant(false),role);
//            }
//        }
//    }
}


void EditingPictureModel::delete_checked_pictures()
{
    QList<QJsonObject>  temp = _selected_data;
    QString picture_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT);
    QString delete_checked_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Checked_For_Export_Or_Delete_Bool);
    QString path_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_Path_TEXT);
    QStringList * paths = new QStringList;
    for(int i=0;i < temp.size();i++)
    {
        const auto & ele = temp.at(i);
        if(ele[delete_checked_key].toBool())
        {
            QString delete_picture_id = ele[picture_key].toString();
            remove(delete_picture_id);
        }
    }
    if(paths->size() > 0)
    {
        std::thread t([paths]{
            for(auto & ele : *paths)
            {
                QFile file(ele);
                if(file.exists())
                    file.remove();
            }
            delete paths;
        });
        t.detach();
    }
    else
    {
        delete paths;
    }
    setCount(get_pics());
}

int EditingPictureModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _selected_data.count();
}

bool EditingPictureModel::update_property(const QString & picture_id, const QJsonObject &new_propertys)
{
    QString picture_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT);
    bool found_in_selected_model = false;
    for(int j = 0; j < _selected_data.size();j++)
    {
        const QJsonObject  & ele = _selected_data.at(j);

        if(ele[picture_key].toString() == picture_id)
        {
            found_in_selected_model = true;
            for(auto it = new_propertys.begin(); it != new_propertys.end();it++)
            {
                int role = roles.key(it.key().toLatin1());
                setData(index(j),it.value().toVariant(),role);
            }
            break;
        }
    }
    return  found_in_selected_model;
}



QVariant EditingPictureModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row < 0 || row >= _selected_data.count()) {
        return QVariant();
    }
    const QJsonObject & ele = _selected_data.at(row);
    QString field = ImagesTable::getInstance()->get_field_name(ImagesTable::Field(role));
    return ele[field];
}


bool EditingPictureModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int row = index.row();
    QVector<int> roles;
    roles.push_back(role);
    if(row < 0 || row >= _selected_data.count()) {
        return false;
    }
    else
    {
        QJsonObject & ele = _selected_data[row];
        QString field = ImagesTable::getInstance()->get_field_name(ImagesTable::Field(role));
        QJsonValue old_value = ele[field];
        QJsonValue new_value  = value.toJsonValue();
        if(old_value == new_value)
            return false;
        else
        {
            ele[field] =new_value ;
            emit dataChanged(index, index,roles);
            return true;
        }
    }
}

//void EditingPictureModel::select(ImagesTable::PictureSoureType picture_soure_type, ImagesTable::PictureLevel picture_level, ImagesTable::PictureHasTeeType picture_has_tee_type, qint64 begin, qint64 end, const QJsonArray &tee_indexs,const QString &tee_type, const QList<QJsonObject> & all_data)
//{
//    clear();
//    for(const auto &  v : all_data )
//    {
//        if(test_filter(v,picture_soure_type,  picture_level,picture_has_tee_type, begin, end, tee_indexs))
//            append(v);
//    }
//}

//bool EditingPictureModel::test_filter(const QJsonObject &picture, ImagesTable::PictureSoureType picture_soure_type, ImagesTable::PictureLevel picture_level, ImagesTable::PictureHasTeeType picture_has_tee_type, qint64 begin, qint64 end, const QJsonArray &tee_indexs)
//{
//    bool picture_type_ok = picture_soure_type == ImagesTable::PictureSoureType::All ||  int(picture_soure_type) == picture[ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Source_Type_integer)].toInt();
//    bool picture_level_ok = picture_level == ImagesTable::PictureLevel::All || int(picture_level) == picture[ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Level_integer)].toInt();
//    bool picture_has_tee_type_ok = false;
//    if(picture_has_tee_type == ImagesTable::PictureHasTeeType::All)
//        picture_has_tee_type_ok = true;
//    else
//    {
//        QString picture_tee_indexs_str = picture[ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Tee_Indexs_TEXT)].toString();
//        QJsonArray picture_tee_indexs;
//        Tools::str_2_jsonarray(picture_tee_indexs_str,picture_tee_indexs);
//        if(picture_has_tee_type == ImagesTable::PictureHasTeeType::NoTeeType)
//        {
//            picture_has_tee_type_ok = picture_tee_indexs.size() == 0;
//        }
//        else
//        {
//            for (const auto & v1 : picture_tee_indexs)
//            {
//                for (const auto & v2 : tee_indexs)
//                {
//                    if(v1.toInt() == v2.toInt())
//                    {
//                        picture_has_tee_type_ok  = true;
//                        break;
//                    }
//                }
//                if(picture_has_tee_type_ok  == true)
//                    break;
//            }
//        }
//    }

//    qint64 pts = picture[ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT)].toString().toLongLong();
//    bool date_ok = pts > begin && pts <end;
//    qDebug() <<  picture_type_ok << picture_level_ok << picture_has_tee_type_ok << date_ok;
//    return  picture_type_ok && picture_level_ok && picture_has_tee_type_ok && date_ok;
//}


QHash<int, QByteArray>  EditingPictureModel::roleNames() const
{
    return roles;
}

int EditingPictureModel::getCount() const
{
    return get_pics();
}

void EditingPictureModel::setCount(int count)
{
    if(_count != count)
    {
        _count = count;
        emit countChanged();
    }
}

void EditingPictureModel::get_checked_files(QStringList &out) const
{

    const QList<QJsonObject> & temp = _selected_data;
    const QString & delete_checked_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Checked_For_Export_Or_Delete_Bool);
    const QString & path_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_Path_TEXT);
    for(int i=0;i < temp.size();i++)
    {
        const QJsonObject & ele = temp.at(i);
        if(ele[delete_checked_key].toBool())
        {
            out << ele[path_key].toString();
        }
    }
}

int EditingPictureModel::get_pics() const
{
    int ret = 0;
    for(int i =0;i < _selected_data.size();i++)
    {
        if(_selected_data.at(i).value("Image_ID_TEXT").toString() != "-1")
        {
            ret++;
        }
    }
    return ret;
}

QStringList EditingPictureModel::get_checked_ids()
{
    QStringList ret;
    for(int k = 0; k < _selected_data.size();k++ )
    {
        QString id =_selected_data.at(k).value("Image_ID_TEXT").toString();
        if(id!= "-1")
        {
            ret.append(id);
        }
    }
    return ret;
}

