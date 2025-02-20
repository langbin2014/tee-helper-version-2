#ifndef PATIENTMANAGER_H
#define PATIENTMANAGER_H
#include<QObject>
#include<QJsonObject>
#include "pcture_manager.h"
#include"../define_tool.h"
#include "../models/patient_model.h"
/**
 * @brief The PatientManager class
 * 病人管理类
 */
class PatientManager : public QObject{
    Q_OBJECT

  //  Q_PROPERTY(int      page_size        READ getPageSize   WRITE setPageSize   NOTIFY page_size_change);
  //  Q_PROPERTY(int      page_count        READ getPageCount  WRITE setPageCount     NOTIFY page_count_change);
 //   Q_PROPERTY(qint64 current_patient READ getCurrent_patient   WRITE setCurrent_patient NOTIFY current_patientChanged)
   //  Q_PROPERTY(QString current_patient_name READ getCurrent_patient_name   WRITE setCurrent_patient_name NOTIFY current_patient_nameChanged)

    PatientManager(QObject *parent= nullptr);

      DEFINE_PRO_NO_AUTO_GET_SET(QString,current_doctor,"")
      DEFINE_PRO_NO_AUTO_GET_SET(QString,current_patient,"")
      DEFINE_PRO_NO_AUTO_GET_SET(long long,current_patient_id,-1)
      DEFINE_PRO_NO_AUTO_GET_SET(QJsonObject,current_patient_info,QJsonObject()) // ??????赋值异常，后续需要跟踪
public:

    Q_INVOKABLE void new_patient(const QJsonObject & patient);
    Q_INVOKABLE void delete_patient(long long pts);
    Q_INVOKABLE void update_patient(qint64 id, const QJsonObject & paiten_new_field);
    Q_INVOKABLE void switch_paitent(qint64 id);
    Q_INVOKABLE QJsonObject get_patient_info(qint64 id);
    Q_INVOKABLE QString second_to_datetime_str(qint64 pts_s,const QString & format);

 //    Q_INVOKABLE int addRecord(QJsonObject object);
 //    Q_INVOKABLE QJsonArray selectRecord();
 //    Q_INVOKABLE bool updateRecord(QJsonObject object, int row_index);
  //   Q_INVOKABLE QJsonArray queryRecord(QJsonObject object, int row_count = 12);
  //   Q_INVOKABLE bool deleteRecord(int record_id);
     Q_INVOKABLE QString getApplicationDirPath();

   //  Q_INVOKABLE QJsonArray nextPage();
  //   Q_INVOKABLE QJsonArray previousPage();

     Q_INVOKABLE void openHelpDocument();



    static PatientManager * get_instance();
    PictureManager * get_picture_manager();

    Q_INVOKABLE QString date_to_year(const QString & date_str);
    Q_INVOKABLE QString date_to_month(const QString & date_str);
    Q_INVOKABLE QJsonObject create_empty_patient();
    Q_INVOKABLE QString get_avatar(qint64 id);
    Q_INVOKABLE  QObject * get_model()
    {
        return model;
    }

  //  qint64 getCurrent_patient() const;
  //  void setCurrent_patient(const qint64 &current_patient);
 //   QString getCurrent_patient_name() const;
  //  Q_INVOKABLE void setCurrent_patient_name(const QString &current_patient_name);

  //  Q_INVOKABLE void setRow_count(int row_count);

    //获取账号列表
 //   Q_INVOKABLE QJsonArray getUerList();
 //   Q_INVOKABLE QString getCurrentUser();

    QString calculateAgeYearAgeMonth(QDate date);

 //   Q_INVOKABLE QString getCurrentPatientNumber(int id);
 //   Q_INVOKABLE QString getCurrentPatientAge(int id);
 //   Q_INVOKABLE QString getCurrentPatientDoctor(int id);
    void load_patients(const QString &doctor);
   Q_INVOKABLE QString get_sql_age_str(int begin,int end);
   Q_INVOKABLE QString get_sql_create_time_str(const QString &info);
   Q_INVOKABLE QString get_full_query_str(const QString &and_str, const QString &key_word);
   Q_INVOKABLE void query_patient(const QString &if_str);
   Q_INVOKABLE void print_current_patient();
   Q_INVOKABLE qint64 birthday_str_to_pts(const QString & info);
signals:
  //  void current_patientChanged();
  //  void current_patient_nameChanged();
    DEFINE_SIG(current_doctor)
    DEFINE_SIG(current_patient)
    DEFINE_SIG(current_patient_info)
    DEFINE_SIG(current_patient_id)
protected:
    private:
  //  qint64 _current_patient_id = -1;
  //  QJsonObject _current_patient_info;
    PictureManager * _picture_manager;
    static PatientManager * instance;
    Patient_Model * model = nullptr;


 //   int _page_size;
   // int _page_count;
  //  int _row_count;
  //  QString _current_patient_name;
    //data base
    //setting
};

#endif // PATIENTMANAGER_H
