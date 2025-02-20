#ifndef REPORT_MANAGER_H
#define REPORT_MANAGER_H

#include<QObject>
#include"../define_tool.h"
#include"../models/report_s_model.h"
#include"../models/current_select_report_model.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmnet/dstorscu.h"
class Report_Manager :public QObject
{
    Q_OBJECT
    DEFINE_PRO_NO_AUTO_GET_SET(qint64,current_report_id,-1)
    DEFINE_PRO_NO_AUTO_GET_SET(qint32,report_maker_type,0) //0是打印机，1是pdf
    DEFINE_PRO_NO_AUTO_GET_SET(QString,report_save_dir,"") //0是打印机，1是pdf
    public:

    Q_INVOKABLE void new_report(const QJsonObject & info);
    Q_INVOKABLE void remove_report(qint64 report_id);
    Q_INVOKABLE void current_report_add_page();
    Q_INVOKABLE void select_report(qint64 id);
    Q_INVOKABLE QObject * get_model(const QString & model);
    Q_INVOKABLE void save_report(qint64 report_id,const QString & mark);
    Q_INVOKABLE void load_patient_all_report(qint64 current_patient_id);
    static Report_Manager * get_instance();

    Q_INVOKABLE QString get_new_report_path();
    Q_INVOKABLE QJsonObject get_new_report_info();

    Q_INVOKABLE void begin_make_pdf();
    Q_INVOKABLE void got_report_one_page(const QImage & pic);
    Q_INVOKABLE void done();
    Q_INVOKABLE QJsonArray get_all_report_page_info();

    Q_INVOKABLE bool select_printer();
    Q_INVOKABLE bool sendDcmDatasetToPacs();
signals:
    DEFINE_SIG(current_report_id)
    DEFINE_SIG(report_maker_type)
    DEFINE_SIG(report_save_dir)
    private:
    Report_Manager(QObject * parent);
    bool initPacsNetwork(DcmStorageSCU &scu);
    void generateOtherDcmDataset(DcmDataset *dataset);
    bool generateDcmDataset(DcmDataset *dataset);

    void generatePatientDcmDataset(DcmDataset *dataset);
    void generateImageDcmDataset(DcmDataset *dataset, const QImage image);
    bool sendDcmDataset(DcmStorageSCU &scu, DcmDataset *data);
    static Report_Manager *_self;
    Report_s_Model * report_s_model;
  //  Current_Select_Report_Model * current_selected_report_model;
    qint64 current_patient_id = -1;
    QList<std::shared_ptr<QImage>> pdf_pages;

    QString printer_name;
    void save_to_pdf();

};

#endif // REPORT_MANAGER_H
