#include "patient_manager.h"
#include"../table/Patient_table.h"
#include <QFile>
#include <QDir>
#include"line_manager.h"
#include <QDesktopServices>
#include <QApplication>
#include<QDebug>
#include"../configure.h"
PatientManager * PatientManager::instance= nullptr;
PatientManager::PatientManager(QObject *parent):QObject(parent)
  // ,_current_patient_id(-1)
  // ,_current_patient_info()
  ,_picture_manager(new PictureManager(this))
  ,model(new Patient_Model(this))
{
    connect(this,&PatientManager::current_patient_infoChanged,this,[this]{
        if(m_current_patient_info.isEmpty() == false)
        {
            int type = m_current_patient_info.value("tee_type_INTEGER").toInt();
            if(type < 0)
                type = 0;
            if(type > 1)
                type =1;
            const char * type_strs[]={"kid","adult"};
            qDebug() << type_strs[type];
            _picture_manager->setCollecting_tee_type(type_strs[type]);
        }

    });
}

void PatientManager::new_patient(const QJsonObject &patient)
{
    qDebug() <<"void PatientManager::new_patient(c" <<patient;
    auto pt = Patient_Table::get_instance();
    pt->add_patent(patient); 
    model->append(patient);

    switch_paitent(patient.value("pts_INTEGER").toInt());


//    set_current_patient_info(patient);
//    set_current_patient(patient.value("name_TEXT").toString());
//    set_current_patient_id(patient.value("pts_INTEGER").toDouble());

}

void PatientManager::delete_patient(long long pts)
{
    auto pt = Patient_Table::get_instance();
    QString if_str =  "doctor_TEXT='"+ m_current_doctor +  "' AND pts_INTEGER=" +QString::number(pts);
    pt->remove(if_str);
    model->remove(pts);
    _picture_manager->on_delete_this_paitent(pts);
    qDebug() <<"void PatientManager::delete_patient(long long pts)";
    if(m_current_patient_id == pts)
    {

        set_current_patient_info(QJsonObject());
        set_current_patient("");
        set_current_patient_id(-1);
    }

}

void PatientManager::update_patient(qint64 id, const QJsonObject &patient)
{
    auto pt = Patient_Table::get_instance();
    QString if_str =  "pts_INTEGER = "+QString::number(id);
    pt->update(if_str,  patient);
    QJsonObject ret =  model->update(id,patient);
    if(id == m_current_patient_id)
    {
        set_current_patient_info(ret);
        set_current_patient(ret.value("name_TEXT").toString());
    }
}

void PatientManager::switch_paitent(qint64 id)
{


    if(id != m_current_patient_id)
    {
        QJsonObject patient = model->get_info(id);

        set_current_patient_info(patient);
        set_current_patient(patient.value("name_TEXT").toString());
        set_current_patient_id(id);


        qDebug() << "切换病人" <<id;
        _picture_manager->load_pictures(id);
        LineManager::get_instance()->load_patien_all_picture_lines(id);
    }
    else
        qDebug() << "相同病人" <<id << m_current_patient_info;
}

QJsonObject PatientManager::get_patient_info(qint64 id)
{
    return model->get_info(id);
}

QString PatientManager::second_to_datetime_str(qint64 pts_s, const QString &format)
{
    return QDateTime::fromSecsSinceEpoch(pts_s).toString(format);
}



//PictureModel *PatientManager::getPictureModel()
//{
//    return _picture_manager->get_picture_model();
//}

//PictureModel::TodayCollectedPictureModel *PatientManager::getToday_picture_model()
//{
//    return _picture_manager->get_picture_model()->get_today_collected_model();
//}

PatientManager *PatientManager::get_instance()
{
    if(instance == nullptr)
    {
        instance =  new PatientManager();
    }
    return  instance;
}

PictureManager *PatientManager::get_picture_manager()
{
    return _picture_manager;
}

QString PatientManager::date_to_year(const QString &date_str)
{
    return Tools::date_to_year(date_str);
}

QString PatientManager::date_to_month(const QString &date_str)
{
    return Tools::date_to_month(date_str);
}

QJsonObject PatientManager::create_empty_patient()
{
    return Patient_Table::get_instance()->create_empty_patient(m_current_doctor);
}

QString PatientManager::get_avatar(qint64 id)
{
    QString file_path = Configure::data_store_root_dir+"/"+id+".png";
    QFile f(file_path);
    if(!f.exists())
        return "qrc:/images/avatar.png";
    else
        return  "file://"+file_path;
}

//void PatientManager::getSearchCreateTimeSql(const QString& key, const QString &value, MedicalRecordDao::QueryItem& item)
//{

//    item.relation = "AND";
//    item.match_type = "BETWEEN";
//    QDate today = QDate::currentDate();
//    item.field_key = key;

//    if (value == "today") {
//        item.value1 = QString("%1 00:00:00").arg(today.toString("yyyy-MM-dd"));
//        item.value2 = QString("%1 23:59:59").arg(today.toString("yyyy-MM-dd"));
//    }
//    else if (value == "yesterday") {
//         QDate yesterday = QDate::currentDate().addDays(-1);
//        item.value1 = QString("%1 00:00:00").arg(yesterday.toString("yyyy-MM-dd"));
//        item.value2 = QString("%1 23:59:59").arg(yesterday.toString("yyyy-MM-dd"));
//    }
//    else if (value == "three_days") {
//        QDate last_three_day = QDate::currentDate().addDays(-3);
//        item.value1 = QString("%1 00:00:00").arg(last_three_day.toString("yyyy-MM-dd"));
//        item.value2 = QString("%1 23:59:59").arg(today.toString("yyyy-MM-dd"));
//    }
//    else if (value == "seven_days") {
//        QDate last_seven_day = QDate::currentDate().addDays(-7);
//        item.value1 = QString("%1 00:00:00").arg(last_seven_day.toString("yyyy-MM-dd"));
//        item.value2 = QString("%1 23:59:59").arg(today.toString("yyyy-MM-dd"));
//    }
//    else if (value == "last_month") {
//        QDate last_moon = QDate::currentDate().addDays(-30);
//        item.value1 = QString("%1 00:00:00").arg(last_moon.toString("yyyy-MM-dd"));
//        item.value2 = QString("%1 23:59:59").arg(today.toString("yyyy-MM-dd"));
//    }
//}

//QString PatientManager::getCurrent_patient_name() const
//{
//    return _current_patient_name;
//}

//void PatientManager::setCurrent_patient_name(const QString &current_patient_name)
//{
//    if(_current_patient_name != current_patient_name)
//    {
//        _current_patient_name = current_patient_name;
//        emit current_patient_nameChanged();
//    }
//}

//void PatientManager::setRow_count(int row_count)
//{
//    _row_count = row_count;
//}

//QJsonArray PatientManager::getUerList()
//{
//    QJsonArray json_array;
////    UserList user_list;
////    _system_service.getUserList(user_list);
////    foreach(UserPtr user, user_list) {
////        QJsonObject object;
////        object.insert("account", user->account);
////        json_array.push_back(object);
////    }
//    json_array.append("-1");
//    json_array.append("-2");
//    return json_array;
//}

//QString PatientManager::getCurrentUser()
//{
//    return "-444";// Global::instance()->getCurrentUser()->account;
//}

QString PatientManager::calculateAgeYearAgeMonth(QDate date)
{
    QDate cur_date = QDate::currentDate();

    int month_num = (cur_date.toString("yyyy").toInt() - date.toString("yyyy").toInt()) * 12
            - (date.toString("MM").toInt() - cur_date.toString("MM").toInt());

    int month = month_num % 12;
    int year = (month_num - month) / 12;
    return QString::number(year) + tr("Y") + QString::number(month)+ tr("M");
}

void PatientManager::load_patients(const QString &doctor)
{
    set_current_doctor(doctor);
    auto pts =  Patient_Table::get_instance()->query("doctor_TEXT='"+doctor+"'");
    model->reset(pts);
    qDebug() << "--------------------------" << pts;
}

QString PatientManager::get_sql_age_str(int begin, int end)
{
    if(end < begin || end < 0 ||begin < 0)
        return "";
    QDateTime today =  QDateTime::currentDateTime();
    qint64 now_s = today.toSecsSinceEpoch();


    qint64 begin_pts = now_s - end*365*24*60*60;
    QString begin_date_time_str = QDateTime::fromSecsSinceEpoch(begin_pts).toString("yyyy-MM-dd");


    qint64 end_pts =  now_s - begin*365*24*60*60;

    QString end_date_str = QDateTime::fromSecsSinceEpoch(end_pts).toString("yyyy-MM-dd");
    QString ret = "birthday_TEXT BETWEEN '"+ begin_date_time_str + "' AND '"+end_date_str+"'";
    return  ret;
}

QString PatientManager::get_sql_create_time_str(const QString &info)
{

    QDateTime today =  QDateTime::currentDateTime();

    QTime time = today.time();
    qint64 today_been_s = time.hour()*60*60+time.minute()*60+time.second();
    qint64 dis_day = 0;
    if(info == "今天" )
    {
        dis_day =0;
    }else if(info == "昨天" )
    {
        dis_day =1;
    }else if(info == "最近三天" )
    {
        dis_day=3;
    }else if(info == "最近七天" )
    {
        dis_day = 7;
    }else if(info == "最近一个月" )
    {
        dis_day = 30;
    }
    qint64 end_pts = today.toSecsSinceEpoch();
    qint64 begin_pts  = end_pts  -( today_been_s + dis_day * 24*60*60);


    QString ret = "pts_INTEGER BETWEEN "+ QString::number(begin_pts) + " AND "+QString::number(end_pts);
    return  ret;
}

QString PatientManager::get_full_query_str(const QString &and_str, const QString &key_word)
{
    return  Patient_Table::get_instance()->get_full_query_str( and_str, key_word);
}

void PatientManager::query_patient(const QString &if_str)
{
    auto pts =  Patient_Table::get_instance()->query(if_str);
    model->reset(pts);
}

void PatientManager::print_current_patient()
{
    qDebug() << m_current_patient_info <<"+++++++++++++";
}

qint64 PatientManager::birthday_str_to_pts(const QString &info)
{
    QDateTime dt = QDateTime::fromString(info,"yyyy-MM-dd");
    return  dt.toSecsSinceEpoch();
}

//QString PatientManager::getCurrentPatientNumber(int id)
//{
//    return "-111";// _system_service.getCurrentPatientNumber(id);
//}

//QString PatientManager::getCurrentPatientAge(int id)
//{
//    return  "-222";
////    QString date_str = _system_service.getCurrentPatientAge(id);
////    if ("" == date_str) {
////        return "";
////    }
////    QDate date = QDate::fromString(date_str, "yyyy-MM-dd");
////    return calculateAgeYearAgeMonth(date);
//}

//QString PatientManager::getCurrentPatientDoctor(int id)
//{
//    return  "-333";//_system_service.getCurrentPatientDoctor(id);;
//}

//qint64 PatientManager::getCurrent_patient() const
//{
//    return _current_patient_id;
//}

//void PatientManager::setCurrent_patient(const qint64 &current_patient)
//{
//    if(_current_patient_id != current_patient)
//    {
//        _current_patient_id = current_patient;
//        emit current_patientChanged();
//    }
//}

//int PatientManager::addRecord(QJsonObject object)
//{
//    QString new_number;
//    int order;
//    _medical_record_service.getNewNumber(new_number, order);
//    qulonglong id = 0;
//    MedicalRecordMapPtr new_record(new MedicalRecordMap());
//    *new_record = object.toVariantMap();
//    if (!(*new_record)["avatar"].isNull()) {
//        Global::instance()->genStorageDir((*new_record)["name"].toString(), new_number);
//        QString data_dir;
//        if (Global::instance()->getCurrentStorageDir(data_dir)) {
//            QString avatar_absolute = (*new_record)["avatar"].toString().replace("file:///", "");
//            QImage image(avatar_absolute);
//            data_dir = data_dir + "/avatar.png";
//            if (image.save(data_dir, "png")) {
//                QDir dir_(QCoreApplication::applicationDirPath());
//                QString path = dir_.relativeFilePath(data_dir);
//                (*new_record)["avatar"] = path;
//            }
//            else {
//                return false;
//            }
//        }
//    }
//    (*new_record)["number"] = new_number;
//    (*new_record)["module"] = "common";
//    (*new_record)["create_time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");
//    qDebug() <<"--------------3333333333333333-----------" <<  *new_record;
//    if (_medical_record_service.addRecord(*new_record, id)) {
//        switch_paitent(id);
//        setCurrent_patient_name(object["name"].toString());
//        return id;
//    }
//    return 0;
//}

//QJsonArray PatientManager::selectRecord()
//{
//    QMutexLocker lock( Global::instance()->getRecursiveMutex());
//    uint64_t total = 0;
//    uint64_t all_total = 0; // 整表总量
//    MedicalRecordDao::QueryParam new_query_param;
//    new_query_param.sort_field = "create_time";
//    new_query_param.is_asc = false;
//    _medical_record_map_list.clear();
//    QJsonArray json_array;
//    int ret = _medical_record_dao->query(new_query_param,  &total, all_total,_medical_record_map_list);
//    for(int i = 0; i< _medical_record_map_list.size(); i++) {
//        QJsonObject json_array_temp;
//        //map中的键将用作JSON对象中的键，QVariant值将被转换为JSON值。
//        json_array_temp = QJsonObject::fromVariantMap(*_medical_record_map_list.at(i));

//        json_array.push_back(json_array_temp);
//    }
//    return json_array;
//}

//bool PatientManager::updateRecord(QJsonObject object, int row_index)
//{
//    MedicalRecordMapPtr medical_record(new MedicalRecordMap());
//    *medical_record = object.toVariantMap();
//    (*medical_record)["id"] = row_index;
//    if ((*medical_record)["avatar"].toString().contains(QString("file:///"))) {
//        Global::instance()->genStorageDir((*medical_record)["name"].toString(),
//                (*medical_record)["number"].toString());
//        QString data_dir;
//        if (Global::instance()->getCurrentStorageDir(data_dir)) {
//            QString avatar_absolute = (*medical_record)["avatar"].toString().replace("file:///", "");
//            data_dir = data_dir + "/avatar.png";
//            QImage image(avatar_absolute);
////            image.save(data_dir,"png");
//            if (image.save(data_dir,"png")) {
//                QDir dir_(QCoreApplication::applicationDirPath());
//                QString path = dir_.relativeFilePath(data_dir);
//                (*medical_record)["avatar"] = path;
//            }
//            else {
//                return false;
//            }
//        }
//    }
//    if (_medical_record_service.updateRecord(*medical_record)) {
//        return true;
//    }
//    return false;
//}

//QJsonArray PatientManager::queryRecord(QJsonObject object, int row_count)
//{
//    QVariantMap map;
//    map = object.toVariantMap();
//    _medical_record_map_list.clear();
//    MedicalRecordDao::QueryParam query_params;
//    QString keyword_value = map["keyword"].toString().trimmed();
//    QVariantMap::iterator it;
//    for(it = map.begin(); it != map.end(); ++it) {
//        qDebug()<<it.key()<<"____--"<<it.value()<<"____--结尾";
//        if (it.value().toString() == "unlimited" || it.key() == "keyword" ||
//                (it.value().toString().isEmpty() && it.key() != "all_key")) {
//                continue;
//        }
//        if (it.key() == "create_time") {
//            MedicalRecordDao::QueryItem item;
//            getSearchCreateTimeSql(it.key(), it.value().toString(), item);
//            query_params.query_items.append(item);
//        }
//        else if (it.key() == "all_key") {//模糊查询
//            if (keyword_value == "")
//                continue;
//            QList<QVariant> list = it.value().toList();
//            MedicalRecordDao::QueryItem item1;
//            MedicalRecordDao::QueryItemList one_big_item;
//            int i = 0;
//            int y = 0;
//            foreach(QVariant key, list) {
//                MedicalRecordDao::QueryItem item;
//                qDebug()<<"_____all_____="<<list[i++];
//                qDebug()<<"_____第几次_____="<<y;
//                item.relation = "OR";
//                item.value1 = keyword_value;
//                item.match_type = "LIKE";
//                item.field_key = key.toString();
//                item.index = 1;
//                one_big_item.append(item);
//            }
//            y++;
//            item1.relation = "AND";
//            item1.sub_items = one_big_item;
//            query_params.query_items.append(item1);
//        }
////        else if (it.key() == "age") {
////            QList<QString> age_list = it.value().toString().split(",");
////            MedicalRecordDao::QueryItem item;
////            item.relation = "AND";
////            item.match_type = "BETWEEN";
////            item.field_key = it.key();
////            item.value1 = age_list.at(0);
////            item.value2 = age_list.at(1);
////            query_params.query_items.append(item);
////        }
//        else if (it.key() == "patient_id_num") {
//            MedicalRecordDao::QueryItem item;
//            item.relation = "AND";
//            item.match_type = "LIKE";
//            item.field_key = it.key();
//            item.value1 = it.value().toString();
//            query_params.query_items.append(item);
//        }
//        else {
//            MedicalRecordDao::QueryItem item;
//            item.relation = "AND";
//            item.match_type = "=";
//            item.value1 = it.value().toString();
//            item.field_key = it.key();
//            query_params.query_items.append(item);
//        }
//    }
//    uint64_t total = 0;
//    uint64_t all_total = 0; // 整表总量
//    query_params.page_size = row_count;
//    query_params.page_num = 1;
//    query_params.sort_field = "create_time";
//    query_params.is_asc = false;
//    _medical_record_service.query(query_params, &all_total, total, _medical_record_map_list);
//    _query_params = query_params;
//    int pageSize = query_params.page_size > 0 ? query_params.page_size : 1;
//    int pageCount = total / pageSize;
//    pageCount = (total % pageSize) ? pageCount + 1 : pageCount;
//    _page_size = query_params.page_num;
//    _page_count = pageCount;
//    QJsonArray json_array;
//    for(int i = 0; i < _medical_record_map_list.size(); i++) {
//        QString date = _medical_record_map_list.at(i).get()->find("birthday").value().toString();
//        _medical_record_map_list.at(i).get()->find("age").value() = calculateAgeYearAgeMonth(QDate::fromString(date, "yyyy-MM-dd"));
//        QJsonObject json_array_temp;
//        //map中的键将用作JSON对象中的键，QVariant值将被转换为JSON值。
//        json_array_temp = QJsonObject::fromVariantMap(*_medical_record_map_list.at(i));

//        json_array.push_back(json_array_temp);
//    }
//    return json_array;
//}

//bool PatientManager::deleteRecord(int record_id)
//{
//    QMutexLocker lock( Global::instance()->getRecursiveMutex());
//    if (_medical_record_dao->del(record_id) == 0) {
//        _picture_manager->on_delete_this_paitent(record_id);
//        return true;
//    }
//    return false;
//}



QString PatientManager::getApplicationDirPath()
{
    return QCoreApplication::applicationDirPath();
}



void PatientManager::openHelpDocument()
{
    QString path = QApplication::applicationDirPath() + "/help_document.pdf";
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}




