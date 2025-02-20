#include "report_manager.h"
#include"../table/report_table.h"
#include"pcture_manager.h"
#include"patient_manager.h"
#include"../configure.h"
#include<QDir>
#include <QApplication>
#include <QPrinter>
#include <QPainter>
#include <QImage>
#include<QPrintDialog>
#include"patient_manager.h"
Report_Manager * Report_Manager::_self = nullptr;
Report_Manager::Report_Manager(QObject *parent):QObject(parent)
{
    report_s_model = new Report_s_Model(this);
    //current_selected_report_model = new Current_Select_Report_Model(this);
}

bool Report_Manager::initPacsNetwork(DcmStorageSCU &scu)
{
    OFList<OFString> ts;
    ts.push_back(UID_LittleEndianExplicitTransferSyntax);
    ts.push_back(UID_BigEndianExplicitTransferSyntax);
    ts.push_back(UID_LittleEndianImplicitTransferSyntax);
    scu.addPresentationContext(UID_FINDStudyRootQueryRetrieveInformationModel, ts);
    scu.addPresentationContext(UID_CTImageStorage, ts);
    scu.addPresentationContext(UID_VerificationSOPClass, ts);

    auto settings = Settings::get_instance();
    // QJsonObject json = _setting_manage.getDicomInfo();
    QString ae = settings->get_pacs_ae_string();
    QString ip = settings->get_pacs_ip_string();
    int port = settings->get_pacs_port_int();
    if (ae.isNull() || ip.isNull() || port  <= 0)
        return  false;

    //    qDebug() << "initPacsNetwork ======= " << json["pacs_port"].toString().toInt() <<  json["pacs_ip"].toString() << json["pacs_ae"].toString();
    scu.setPeerPort(port);//设置端口  21111
    std::string std_str = ip.toStdString();
    scu.setPeerHostName(std_str.c_str());//设置 IPstd_str.c_str() "127.0.0.1"
    std_str = QString("SCIVIEW").toStdString();
    scu.setAETitle(std_str.c_str());//设置自己的AETitle  std_str.c_str() "STORESCU"
    std_str = ae.toStdString();
    scu.setPeerAETitle(std_str.c_str());//设置服务器的AETitle   "ANY-SCP"
    scu.initNetwork();  //初始化网络
    scu.negotiateAssociation();
    return  true;
}

void Report_Manager::generateOtherDcmDataset(DcmDataset *dataset)
{
    char uid[100];
    dataset->putAndInsertString(DCM_ImageType,"ORIGINAL\\PRIMARY\\TEE\\0011");
    dataset->putAndInsertString(DCM_SOPClassUID, UID_SecondaryCaptureImageStorage);//UID_RETIRED_UltrasoundImageStorage);
    dataset->putAndInsertString(DCM_SOPInstanceUID, dcmGenerateUniqueIdentifier(uid, SITE_INSTANCE_UID_ROOT));
    // relation group  关系 组
    dataset->putAndInsertString(DCM_StudyInstanceUID,"999.999.2.19941105.112000");
    dataset->putAndInsertString(DCM_SeriesInstanceUID,"999.999.2.19941105.112000.2");
    dataset->putAndInsertString(DCM_SeriesNumber,"2");
    dataset->putAndInsertString(DCM_AccessionNumber,"1");
    dataset->putAndInsertString(DCM_InstanceNumber,"1");
    dataset->putAndInsertUint16(DCM_SamplesPerPixel,3);
    dataset->putAndInsertUint16(DCM_BitsAllocated,8);
    dataset->putAndInsertUint16(DCM_BitsStored,8);
    dataset->putAndInsertUint16(DCM_HighBit,7);
    dataset->putAndInsertUint16(DCM_PixelRepresentation,0);
    dataset->putAndInsertUint16(DCM_PlanarConfiguration,0);
    dataset->putAndInsertString(DCM_PixelAspectRatio,"4\\3");
    //其他
    dataset->putAndInsertString(DCM_Manufacturer,"ACME product");
    dataset->putAndInsertString(DCM_ReferringPhysicianName,"ANONY");
    dataset->putAndInsertString(DCM_StudyDescription,"STUDY description");
    dataset->putAndInsertString(DCM_SeriesDescription,"SERIES DESCRIPTION");
    dataset->putAndInsertString(DCM_StageNumber,"1");
    dataset->putAndInsertString(DCM_NumberOfStages,"1");
    dataset->putAndInsertString(DCM_ViewNumber,"1");
    dataset->putAndInsertString(DCM_NumberOfViewsInStage,"1");
}

bool Report_Manager::generateDcmDataset(DcmDataset *dataset)
{
    generateOtherDcmDataset(dataset);
    generatePatientDcmDataset(dataset);
    return true;
}

void Report_Manager::generatePatientDcmDataset(DcmDataset *dataset)
{

    auto pt = PatientManager::get_instance();
    QJsonObject info =pt->get_current_patient_info();
    auto settings = Settings::get_instance();

    if (info.empty()) {
        return ;
    }

    //    address_TEXT,
    //    avatar_TEXT, //头像
    //    birthday_TEXT,
    //   // birthday_INTEGER,
    //    complaint_of_illness_TEXT, //诉求
    //    id_TEXT,//年月日+表里面的数量+1生产
    //    watch_date_TEXT, //年月日,用于生成id
    //    pts_INTEGER, //毫秒时间戳//日期查询,唯一id
    //    diagnosis_id_num_INTEGER, //第几个病人
    //    doctor_TEXT,
    //    sex_INTEGER, //"", "男", "女"
    //    name_TEXT,
    //    phone_TEXT,
    //    tee_type_INTEGER, //0小孩，1成人

    const char * sex[3] = {"", "男", "女"};

    // QString date = QDate::fromString(_cur_report_info["date"].toString(), "yyyy-MM-dd").toString("yyyy-MM-dd");
    //    QString time = QDateTime::fromString(_cur_report_info["check_date"].toString(), "yyyy-MM-dd HH:mm:ss").toString("HH:mm:ss");

    dataset->putAndInsertString(DCM_StudyID, QString::number(/*info["pts_INTEGER"].toInt()*/ m_current_report_id).toStdString().c_str()); //检测号

    QDateTime dt= QDateTime::currentDateTime();
    QString date = dt.toString("yyyy-MM-dd HH:mm:ss");
    dataset->putAndInsertString(DCM_StudyDate, date.toStdString().c_str());//检测日期
    //    dataset->putAndInsertString(DCM_StudyTime, time.toStdString().c_str());//检测时间
    dataset->putAndInsertString(DCM_Modality,"ES");// 检测项目 ES(内窥镜)
    //病人信息
    //    dataset->putAndInsertString(DCM_PatientID, _cur_report_info["outpatient_service_no"].toString().toStdString().c_str());//病人id
    dataset->putAndInsertString(DCM_PatientName, info["name_TEXT"].toString().toStdString().c_str());//病人名称


    QString sex_str = settings->tran(sex[info.value("sex_INTEGER").toInt()]);
    dataset->putAndInsertString(DCM_PatientSex, sex_str.toStdString().c_str()); //性别

    QString year = pt->date_to_year(info.value("birthday_TEXT").toString());

    dataset->putAndInsertString(DCM_PatientAge, year.toStdString().c_str());//年龄
    //    dataset->putAndInsertString(DCM_PatientAddress, _cur_report_info["home_address"].toString().toStdString().c_str());//住址

    QJsonObject rp_info = report_s_model->get_info(m_current_report_id);
    QString mark = rp_info.value("mark_TEXT").toString();

    dataset->putAndInsertString(DCM_PatientState, mark.toStdString().c_str());//表现（状态）
    dataset->putAndInsertString(DCM_PatientOrientation, mark.toStdString().c_str());//术后诊断(情况)
    //    dataset->putAndInsertString(DCM_StudyDescription, _cur_report_info["description"].toString().toStdString().c_str());//手术过程（检查描述）
    //    dataset->putAndInsertString(DCM_PatientTelephoneNumbers, _cur_report_info["tel"].toString().toStdString().c_str());//电话
}

void Report_Manager::generateImageDcmDataset(DcmDataset *dataset, const QImage image)
{
    dataset->putAndInsertString(DCM_PhotometricInterpretation, "RGB");//图片数据保存格式
    int width = image.width();
    int height = image.height();
    dataset->putAndInsertUint16(DCM_Columns,width);// 图片宽度（列数）
    dataset->putAndInsertUint16(DCM_Rows,height);//   图片高度（行数）

    unsigned long size = width*height;
    BYTE* pData=new BYTE[size*3];
    memset(pData, 0, size*3);
    for(int y=0; y < height; y++){//遍历高
        for(int x=0; x<width; x++)//遍历宽
        {
            pData[y*width*3+x*3] = QColor(image.pixel(x,y)).red();
            pData[y*width*3+x*3 + 1] = QColor(image.pixel(x,y)).green();
            pData[y*width*3+x*3 + 2] = QColor(image.pixel(x,y)).blue();
        }
    }
    dataset->putAndInsertUint8Array(DCM_PixelData, pData, size*3);
}

bool Report_Manager::sendDcmDataset(DcmStorageSCU &scu, DcmDataset *data)
{
    Uint8 context = scu.findPresentationContextID(UID_CTImageStorage, UID_LittleEndianExplicitTransferSyntax);
    if (context == 0) {
        context = scu.findPresentationContextID(UID_CTImageStorage, UID_BigEndianExplicitTransferSyntax);
    }
    if (context == 0) {
        context = scu.findPresentationContextID(UID_CTImageStorage, UID_LittleEndianImplicitTransferSyntax);
    }

    Uint16 status = 9999;

    OFCondition cond = scu.sendSTORERequest(context, NULL, data, status);
    return cond.good();
}

void Report_Manager::save_to_pdf()
{

    // 创建QPrinter对象，指定输出为PDF文件
    QPrinter printer(QPrinter::PrinterResolution);

    qDebug() << m_report_maker_type;
    if(m_report_maker_type == 0)
    {
        printer.setOutputFormat(QPrinter::NativeFormat);
        printer.setPrinterName(printer_name);
    }
    else
    {
        printer.setOutputFormat(QPrinter:: PdfFormat);
        QString file_name = m_report_save_dir +"/"+ PatientManager::get_instance()->get_current_patient() +"_report_"+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss")+".pdf";
        qDebug() << file_name;
        printer.setOutputFileName(file_name.toUtf8().data()); // 输出的PDF文件名
    }


    // 设置页面尺寸
    printer.setPageSize(QPrinter::A4); // 设置为A4纸张大小

    // 创建QPainter对象，用于绘制PDF页面
    QPainter painter;
    painter.begin(&printer);

    // 加载QImage对象

    for(int i =0; i < pdf_pages.size();i++)
    {
        QImage image = *pdf_pages.at(i); // 替换为你的QImage对象

        // 获取页面尺寸

        QSize pageSize = printer.pageRect().size();

        // 计算图片的缩放比例
        qreal scale = qMin(pageSize.width() / qreal(image.width()), pageSize.height() / qreal(image.height()));

        // 计算图片的绘制区域
        int imageWidth = scale * image.width();
        int imageHeight = scale * image.height();
        QRectF targetRect((pageSize.width() - imageWidth) / 2.0, (pageSize.height() - imageHeight) / 2.0, imageWidth, imageHeight);

        // 将图片绘制到PDF页面上
        painter.drawImage(targetRect, image);

        // 结束绘制
        if(i != pdf_pages.size() -1)
            printer.newPage();
    }

    painter.end();
}

void Report_Manager::new_report(const QJsonObject & info)
{
    int report_id = info.value("report_id_pts_INTEGER").toInt();
    QString source_pic_url = get_new_report_path();
    source_pic_url = source_pic_url.replace("file:///","");
    QImage pic(source_pic_url);

    QString dir = Settings::get_instance()->get_report_picture_dir();
    QDir dir_checker;

    if(dir_checker.exists(dir) == false)
    {
        Tools::make_path(dir, 0);
    }


    QString target_pic_url = dir+"/"+info.value("report_pic_url_TEXT").toString();
    pic.save(target_pic_url,"png");








    //    auto pt_m = PatientManager::get_instance();
    // auto  pic_m = pt_m->get_picture_manager();
    auto tb = Report_Table::get_instance();

    report_s_model->append(info);
    tb->add_report(info);

    select_report(report_id);
}

void Report_Manager::remove_report(qint64 report_id)
{

    QJsonObject info = report_s_model->remove(report_id);
    QString if_str = "report_id_pts_INTEGER=" +QString::number(report_id);
    Report_Table::get_instance()->remove(if_str);
    //    if(report_id == m_current_report_id && report_id != -1)
    //    {
    //        current_selected_report_model->clear();
    //    }

    if(info.isEmpty()  == false)
    {
        auto settings = Settings::get_instance();
        QString pic =   settings->get_report_picture_dir()+"/"+info.value("report_pic_url_TEXT").toString();
        QFile f(pic);
        f.remove();
    }
}

void Report_Manager::current_report_add_page()
{
    //current_selected_report_model->new_page();
}

void Report_Manager::select_report(qint64 id)
{
    QJsonObject info =  report_s_model->get_info(id);
    if(info.isEmpty())
        set_current_report_id(-1);
    else
    {
        set_current_report_id(info.value("report_id_pts_INTEGER").toInt());
        //  QJsonArray ret;
        //   Tools::str_2_jsonarray(info.value("report_content_json_array_TEXT").toString(),ret);
        //   current_selected_report_model->reset(ret);
    }
}

QObject *Report_Manager::get_model(const QString &model)
{
    if(model == "report_s_model")
    {
        return report_s_model;
    }
    else if(model == "current_selected_report_model")
    {
        //  qDebug() <<  "current_selected_report_model;" << current_selected_report_model;
        return nullptr;//current_selected_report_model;
    }
    else
        return nullptr;
}

void Report_Manager::save_report(qint64 report_id, const QString &mark)
{

    QJsonObject info;
    info["mark_TEXT"] = mark;
    auto pt_m = PatientManager::get_instance();
    auto tb  = Report_Table::get_instance();
    QString if_str = "doctor_TEXT='"+pt_m->get_current_doctor()+ "' AND report_id_pts_INTEGER=" +QString::number(report_id);
    tb->update(if_str,info);
}


void Report_Manager::load_patient_all_report(qint64 current_patient_id)
{
    if(current_patient_id == -1)
    {
        report_s_model->clear();
        //current_selected_report_model->clear();
        return;
    }else{

        if(current_patient_id != this->current_patient_id)
        {
            this->current_patient_id = current_patient_id;
            auto pt_m = PatientManager::get_instance();
            auto tb  = Report_Table::get_instance();
            QString if_str = "doctor_TEXT='"+pt_m->get_current_doctor()+ "' AND patient_id_INTEGER=" +QString::number(current_patient_id);
            auto ret = tb->query(if_str);

            report_s_model->reset(ret);
            //    current_selected_report_model->clear();
        }
    }
}

Report_Manager *Report_Manager::get_instance()
{
    if(_self == nullptr)
    {
        _self= new Report_Manager(nullptr);
    }
    return  _self;
}


QString Report_Manager::get_new_report_path()
{
    return "file:///" +QCoreApplication::applicationDirPath()+"/new_report.png";
}

QJsonObject Report_Manager::get_new_report_info()
{
    auto pt_m = PatientManager::get_instance();
    auto tb = Report_Table::get_instance();
    QJsonObject info =  tb->create_empty_report(pt_m->get_current_patient_id(),pt_m->get_current_doctor(),pt_m->get_current_patient());
    return info;
}

void Report_Manager::begin_make_pdf()
{

    pdf_pages.clear();
}



void Report_Manager::got_report_one_page(const QImage &pic)
{
    pdf_pages.append(std::make_shared<QImage>(pic));
    qDebug() << pic;
}

void Report_Manager::done()
{
    save_to_pdf();
}

QJsonArray Report_Manager::get_all_report_page_info()
{
    return  report_s_model->get_all_info();
}

bool Report_Manager::select_printer()
{
    //QPrinter printer(QPrinter::HighResolution);
    //printer.setFromTo(1, _qtrpt->pageList.size());
    QPrintDialog dlg(nullptr, nullptr);
    if (dlg.exec() == QDialog::Rejected) return false;
    else
    {
        qDebug() << dlg.printer()->printerName();
        printer_name = dlg.printer()->printerName();
        return true;
    }
    //    _qtrpt->getPrinter()->setPrinterName(dlg.printer()->printerName());
    //    _qtrpt->getCustomPreview()->print();
}

bool Report_Manager::sendDcmDatasetToPacs()
{
    bool send_successful = false;
    DcmStorageSCU scu;
    if (!initPacsNetwork(scu)) {
        return false;
    }
    DcmFileFormat fileformat;
    DcmDataset *dataset = fileformat.getDataset();
    if (generateDcmDataset(dataset)) {


        QJsonObject rp_info = report_s_model->get_info(m_current_report_id);

        auto settings =   Settings::get_instance();
        QString url = settings->get_report_picture_dir()+"/"+rp_info.value("report_pic_url_TEXT").toString();

        QImage image(url);
        image = image.convertToFormat(QImage::Format::Format_RGB888);
        qDebug() << image;
        if (image.isNull()) {
            send_successful = sendDcmDataset(scu, dataset);
        } else {
            generateImageDcmDataset(dataset, image);
            send_successful = sendDcmDataset(scu, dataset);
        }
    }
    scu.releaseAssociation();
    return  send_successful;
}
