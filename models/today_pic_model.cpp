#include "today_pic_model.h"
#include "picture_model.h"
#include "c++/table/images_table.h"
#include "c++/tools.h"
#include <QFile>
#include"c++/log.h"
Today_Pic_Model::Today_Pic_Model(QObject *parent):QAbstractListModel(parent)
{
    _today_pts = Tools::get_1970_ms();
    for(int i =0;i < int(ImagesTable::Field::End_Field);i++ )
    {
        if(i == int(ImagesTable::Field::End_DataBase_Field))
            continue;
        roles[i] = ImagesTable::getInstance()->get_field_name(ImagesTable::Field(i)).toLatin1();
    }
    _checked_for_paint_list_role =  roles.key(ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Checked_For_Add_To_Paint_List_Bool).toLatin1());
    _picture_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT);
    //     qDebug() <<roles;
}

void Today_Pic_Model::clear()
{
    beginResetModel();
    _selected_data.clear();
    endResetModel();
    setCount(0);
}

void Today_Pic_Model::set_all_checked_for_on_editing(bool checked)
{
    QString checked_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Checked_For_Add_To_Paint_List_Bool);
    int role = roles.key(checked_key.toLatin1());
    for(int i =0; i < _selected_data.size();i++)
    {
        const QJsonObject & ele = _selected_data.at(i);
         bool now = ele[checked_key].toBool();
        if(now != checked)
        {
            setData(index(i),QVariant(checked),role);
        }
    }
}

void Today_Pic_Model::set_one_checked_for_on_editing(const QString &picture_id,bool checked)
{
    QString checked_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Checked_For_Add_To_Paint_List_Bool);
    int role = roles.key(checked_key.toLatin1());
    QString picture_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT);
    for(int i =0; i < _selected_data.size();i++)
    {
        const QJsonObject & ele = _selected_data.at(i);
         QString this_id = ele[picture_key].toString();
        if(this_id == picture_id)
        {
            setData(index(i),QVariant(checked),role);
            break;
        }
    }
}

void Today_Pic_Model::set_one_checked_for_on_editing(int index_,bool checked)
{
    QString checked_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Checked_For_Add_To_Paint_List_Bool);
    int role = roles.key(checked_key.toLatin1());
    setData(index(index_),QVariant(checked),role);
}

//void Today_Pic_Model::clear_full_list()
//{
////    clear();
////    _today_collected_model->clear();
////    for(auto v : _all_data)
////        delete v;
////    _all_data.clear();
//}

void Today_Pic_Model::checked_export_or_delete(bool checked)
{
    for(int i = 0; i < _selected_data.size();i++)
    {
        setData(index(i),QVariant(checked),int(ImagesTable::Field::Checked_For_Export_Or_Delete_Bool));
    }
}

//void Today_Pic_Model::append_to_full_list( QJsonObject *info)
//{
////    qint64 pts = (*info)[ImagesTable::getInstance()->get_field_name(ImagesTable::Field(ImagesTable::Field::Image_ID_TEXT))].toString().toLongLong();
////    _all_data.append(info);
////    if(pts > _today_pts)
////        _today_collected_model->append(info);
//}

void Today_Pic_Model::append(const QJsonObject &info)
{
    int size = _selected_data.size();
    beginInsertRows(QModelIndex(), size , size);
    _selected_data.append(info);
    endInsertRows();
    setCount(_selected_data.size());
    QString str = info.value("Image_ID_TEXT").toString();
    qDebug() << "插入" << str.mid(str.length()-4);
}

void Today_Pic_Model::remove_first()
{
    if(_selected_data.size() == 0)
        return;
    QString str = _selected_data.first().value("Image_ID_TEXT").toString();
    qDebug() << "移出" << str.mid(str.length()-4);
    beginRemoveRows(QModelIndex(),0, 0);
    _selected_data.removeFirst();
    endRemoveRows();
    setCount(_selected_data.size());

}

void Today_Pic_Model::remove(const QString &picture_id)
{
    QString key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT);
    for(int i =0 ;i < _selected_data.size();i ++)
    {
       const QJsonObject & ele = _selected_data.at(i);
       const QString & temp_picture_id = ele[key].toString();
        if(picture_id == temp_picture_id)
        {
            beginRemoveRows(QModelIndex(), i, i);
            _selected_data.removeAt(i);
            endRemoveRows();
            break;
        }
    }
    setCount(_selected_data.size());
}

//void Today_Pic_Model::remove(QJsonObject *info)
//{
//    for(int i =0 ;i < _selected_data.size();i ++)
//    {
//        const QJsonObject & ele = _selected_data.at(i);
//        if(ele == info)
//        {
//            beginRemoveRows(QModelIndex(), i, i);
//            _selected_data.removeAt(i);
//            endRemoveRows();
//            break;
//        }
//    }
//}

void Today_Pic_Model::delete_checked_pictures(std::map<QString,QString> & out)
{
    QList<QJsonObject>  temp = _selected_data;
    QString picture_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT);
    QString delete_checked_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Checked_For_Export_Or_Delete_Bool);
    QString path_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_Path_TEXT);

    for(int i=0;i < temp.size();i++)
    {
        const auto & ele = temp.at(i);
        if(ele[delete_checked_key].toBool())
        {
            const QString & selected_picture_id = ele[picture_key].toString();
            remove(selected_picture_id);
            QString path = ele[path_key].toString();
            QString id = ele[picture_key].toString();
             out[id] = path;
        }
    }

    setCount(_selected_data.size());
}

const QList<QJsonObject> &Today_Pic_Model::get_data() const
{
    return _selected_data;
}

int Today_Pic_Model::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _selected_data.count();
}

bool Today_Pic_Model::update_property(const QString & picture_id, const QJsonObject & new_propertys)
{
    QString picture_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT);
    bool found_in_selected_model = false;
   for(int j = 0; j < _selected_data.size();j++)
   {
       const auto & ele = _selected_data.at(j);
       const QString & ele_picture_id = ele[picture_key].toString();
       if(ele_picture_id  == picture_id)
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

void Today_Pic_Model::checked_thoes_ids(const QStringList &picture_id)
{
   QString picture_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT);
   QString Checked_For_Add_To_Paint_List_Bool_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Checked_For_Add_To_Paint_List_Bool);

   int role = roles.key(Checked_For_Add_To_Paint_List_Bool_key.toLatin1());
   QVector<int> roles;
   roles.append(role);
   for(int j = 0; j < _selected_data.size();j++)
   {
        auto & ele = _selected_data[j];
        QString ele_id = ele.value(picture_key).toString();
        bool found = false;
        for(int k =0;k<picture_id.size();k++)
        {
           if(ele_id  == picture_id.at(k))
           {
                 found = true;
                 break;
           }
        }
        ele[Checked_For_Add_To_Paint_List_Bool_key] = found;
        dataChanged(index(j),index(j),roles);
   }
}

QVariant Today_Pic_Model::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row < 0 || row >= _selected_data.count()) {
        return QVariant();
    }
    const QJsonObject & ele = _selected_data.at(row);
    QString field = ImagesTable::getInstance()->get_field_name(ImagesTable::Field(role));
    return ele[field];
}


bool Today_Pic_Model::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int row = index.row();
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
            emit dataChanged(index, index);
            if(role == _checked_for_paint_list_role)
            {
                emit checked_for_paint_list_changed(new_value.toBool(),ele[_picture_key].toString());
            }
            return true;
        }
    }
}

//void Today_Pic_Model::select(ImagesTable::PictureSoureType picture_soure_type, ImagesTable::PictureLevel picture_level, ImagesTable::PictureHasTeeType picture_has_tee_type, qint64 begin, qint64 end, const QJsonArray &tee_indexs,const QString &tee_type, const QList<QJsonObject> & all_data)
//{
//    clear();
//    for(auto  v : all_data )
//    {
//        if(test_filter(v,picture_soure_type,  picture_level,picture_has_tee_type, begin, end, tee_indexs))
//            append(v);
//    }
//}

//bool Today_Pic_Model::test_filter(const QJsonObject &picture, ImagesTable::PictureSoureType picture_soure_type, ImagesTable::PictureLevel picture_level, ImagesTable::PictureHasTeeType picture_has_tee_type, qint64 begin, qint64 end, const QJsonArray &tee_indexs)
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


QHash<int, QByteArray>  Today_Pic_Model::roleNames() const
{
    return roles;
}

int Today_Pic_Model::getCount() const
{
    return _count;
}

void Today_Pic_Model::setCount(int count)
{
    if(_count != count)
    {
        _count = count;
        emit countChanged();
    }
}

void Today_Pic_Model::get_checked_files(QStringList &out) const
{

    const QList<QJsonObject> & temp = _selected_data;
    const QString & delete_checked_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Checked_For_Export_Or_Delete_Bool);
    const QString & id_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT);
    for(int i=0;i < temp.size();i++)
    {
        const QJsonObject & ele = temp.at(i);
        if(ele[delete_checked_key].toBool())
        {
            out << ele[id_key].toString();
        }
    }
}

//QJsonObject * Today_Pic_Model::update_property(const QString & picture_id,const QJsonObject & new_propertys)
//{
//    QString picture_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT);
//    QJsonObject * ret = nullptr;

//    for(int i = 0;i < _all_data.size();i++)
//    {
//        QJsonObject * ele = _all_data.at(i);
//        if((*ele)[picture_key].toString() == picture_id)
//        {
//            ret = ele;
//             bool found_in_selected_model = false;
//            for(int j = 0; j < _selected_data.size();j++)
//            {
//                QJsonObject * ele_selected = _selected_data.at(j);
//                if(ele_selected == ele)
//                {
//                    found_in_selected_model = true;
//                    for(auto it = new_propertys.begin(); it != new_propertys.end();it++)
//                    {
//                          int role = roles.key(it.key().toLatin1());
//                          setData(index(j),it.value().toVariant(),role);
//                    }
//                    break;
//                 }
//            }
//            if(found_in_selected_model == false)
//            {
//                for(auto it = new_propertys.begin(); it != new_propertys.end();it++)
//                {
//                      (*ele)[it.key()] = it.value();
//                }
//            }
//            break;
//        }
//    }
//    return  ret;

//}

////////////////////////////////////////////////////////今天收集到的图片集合

//Today_Pic_Model::TodayCollectedToday_Pic_Model::TodayCollectedToday_Pic_Model(QObject *parent):QAbstractListModel(parent)
//{
//    roles[int(ImagesTable::Field::Image_ID_TEXT)] = ImagesTable::getInstance()->get_field_name(ImagesTable::Field(ImagesTable::Field::Image_ID_TEXT)).toLatin1();
//    roles[int(ImagesTable::Field::Patient_ID_INTEGER)] = ImagesTable::getInstance()->get_field_name(ImagesTable::Field(ImagesTable::Field::Patient_ID_INTEGER)).toLatin1();
//    roles[int(ImagesTable::Field::Image_Path_TEXT)] = ImagesTable::getInstance()->get_field_name(ImagesTable::Field(ImagesTable::Field::Image_Path_TEXT)).toLatin1();
//    roles[int(ImagesTable::Field::Source_Type_integer)] = ImagesTable::getInstance()->get_field_name(ImagesTable::Field(ImagesTable::Field::Source_Type_integer)).toLatin1();
//}

//int Today_Pic_Model::TodayCollectedToday_Pic_Model::rowCount(const QModelIndex &parent) const
//{
//    Q_UNUSED(parent);
//    return _selected_data.count();
//}

//QVariant Today_Pic_Model::TodayCollectedToday_Pic_Model::_data(const QModelIndex &index, int role) const
//{
//    int row = index.row();
//    if(row < 0 || row >= _selected_data.count()) {
//        return QVariant();
//    }
//    const QJsonObject * ele = _selected_data[row];
//    QString field = ImagesTable::getInstance()->get_field_name(ImagesTable::Field(role));
//    return (*ele)[field];
//}

//void Today_Pic_Model::TodayCollectedToday_Pic_Model::clear()
//{
//    beginResetModel();
//    _selected_data.clear();
//    endResetModel();
//}

//void Today_Pic_Model::TodayCollectedToday_Pic_Model::append(QJsonObject *info)
//{
//    int size = _selected_data.size();
//    beginInsertRows(QModelIndex(), size, size);
//    _selected_data.append(info);
//    endInsertRows();
//}

//QHash<int, QByteArray> Today_Pic_Model::TodayCollectedToday_Pic_Model::roleNames() const
//{
//    return roles;
//}
