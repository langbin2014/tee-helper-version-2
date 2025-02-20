#include "pcture_manager.h"
#include "c++/table/images_table.h"
#include "c++/tools.h"
#include <QFile>
#include<core/image_core.h>
#include <QApplication>
#include"c++/image_factor.h"
#include"c++/configure.h"
#include"line_manager.h"
#include"c++/log.h"
//dcmtk
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"
//dcmtk
#include"c++/settings.h"
void PictureManager::reset_collected_page_picture_filter()
{
    QDateTime now = QDateTime::currentDateTime();
    auto b= now.addDays(-365).toMSecsSinceEpoch(); //2周前
    now.setTime(QTime(23,59));
    auto e= now.toMSecsSinceEpoch(); //2周前

    setCollected_source_type(int(ImagesTable::PictureSoureType::All));
    setCollected_has_tee_type(int(ImagesTable::PictureHasTeeType::All));
    setCollected_date_begin(b);
    setCollected_date_end(e);

    _fillter_for_patient_page_collected_picture->reinit();
    _fillter_for_patient_page_collected_picture->drop_signal = true;
    emit collected_source_typeChanged();
    //  emit collected_levelChanged();
    emit collected_has_tee_typeChanged();
    emit collected_date_beginChanged();
    emit collected_date_endChanged();
    emit collected_tee_typeChanged();
    _fillter_for_patient_page_collected_picture->drop_signal = false;
    emit  collected_choosed_tee_indexsChanged();
    emit reset_fillter_ui();
}

PictureManager::PictureManager(QObject *parent):QObject(parent),
    _collected_picture_model(new PictureModel(this)),
    _today_picture_model(new TodayCollectedPictureModel(this)),
    _editing_picture_model(new EditingPictureModel(this)),
    _today_pts(Tools::get_1970_ms()),
    _patient_id(-1),
    //_tee_indexs_selecter_provider_for_collecting_image(new TeeIndexsSelecterImageProvider),
    //_tee_indexs_selecter_provider_for_picture_info(new TeeIndexsSelecterImageProvider),
    _fillter_for_patient_page_collected_picture(new FillterForPatientPageCollectedPicture),
    _editing_iamge_provider(new EditingImageProvider),
    _fixed_image(new  Fixed_Image),
    _usb_reader(new UsbReader(this)),
    _expose_second_value("0.00")

  //_rui_hua_thread(new RuiHuaThread(this))
{
    QJsonObject params;
    Configure::get_default_visible_propertys(params);
    QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat);
    params = settings.value("default_visible_params",params).toJsonObject();
    Configure::set_default_visible_propertys(params);
    //    connect(_rui_hua_thread,&RuiHuaThread::finish,this,[this](const QString &id,int value){
    //        QJsonObject new_property;
    //        QString rui_hua_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Rui_Hua_integer);
    //        QString url_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Url_str);
    //        new_property[rui_hua_key] = value;
    //        update_picture_property_any_where(id,new_property);
    //        auto it = _pictures_old_visible_property.find(id);
    //        if(it != _pictures_old_visible_property.end())
    //        {
    //            it->second._queue.push_back(new_property);
    //        }
    //        new_property = QJsonObject();
    //        new_property[url_key] = "image://editing_images/"+ id+ "/"+QString::number(Tools::get_1970_ms());
    //        only_update_property_to_model(id, new_property);
    //    });
    //   _rui_hua_thread->start();
    //_tee_indexs_selecter_provider_for_collecting_image->init();
    // _tee_indexs_selecter_provider_for_picture_info->init();
    QObject::connect(this,&PictureManager::collected_source_typeChanged,this,&PictureManager::update_selected_picture);
    //  QObject::connect(this,&PictureManager::collected_levelChanged,this,&PictureManager::update_selected_picture);
    QObject::connect(this,&PictureManager::collected_has_tee_typeChanged,this,&PictureManager::update_selected_picture);
    QObject::connect(this,&PictureManager::collected_date_beginChanged,this,&PictureManager::update_selected_picture);
    QObject::connect(this,&PictureManager::collected_date_endChanged,this,&PictureManager::update_selected_picture);
    //  QObject::connect(this,&PictureManager::collected_choosed_tee_indexsChanged,this,&PictureManager::update_selected_picture);
    //   QObject::connect(this,&PictureManager::collected_tee_typeChanged,this,&PictureManager::update_selected_picture);

    QObject::connect(this,&PictureManager::collecting_tee_typeChanged,this,[this]{
        // _tee_indexs_selecter_provider_for_collecting_image->select(_collecting_tee_type);
        // collecting_picture_url_change(_tee_indexs_selecter_provider_for_collecting_image->get_url());//切换图片刷新
    });
    QObject::connect(_collected_picture_model,&PictureModel::checked_for_paint_list_changed,this,[this](bool checked,const QString & picture_id){
        QString picture_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT);
        QString checked_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Checked_For_Add_To_Paint_List_Bool);
        for(int i =0; i < _all_data.size();i++)
        {
            QJsonObject &ele = _all_data[i];
            if(ele[picture_key].toString() == picture_id)
            {
                ele[checked_key] = checked;
                break;
            }
        }
    });
    connect(this,&PictureManager::current_editing_picture_stateChanged,this,[this]{
        on_image_state_changed();
    });
    QFile fram(":/shaders/editing.fram");
    QFile ver(":/shaders/editing.ver");
    bool ok = ver.open(QIODevice::ReadOnly);
    if(ok)
        _ver_shader = QString(ver.readAll());
    ok = fram.open(QIODevice::ReadOnly);
    if(ok)
        _fram_shader = QString(fram.readAll());
    _expose_second_index =  settings.value("_expose_second_index",0).toInt();
    _expose_cm = settings.value("_expose_cm","0.0").toString();
    _voltage_v =  settings.value("_voltage_v","0.0").toString();
    _electricity_a = settings.value("_electricity_a","0.0").toString();
    _source_index = settings.value("_source_index",1).toInt();


    _show_transformation_bar = settings.value("_show_transformation_bar",true).toBool();
    _show_fix_image_bar = settings.value("_show_fix_image_bar",true).toBool();
    _show_filter_bar = settings.value("_show_filter_bar",true).toBool();
    _show_color_space_bar = settings.value("_show_color_space_bar",true).toBool();
    _show_meter_bar = settings.value("_show_meter_bar",true).toBool();
    _show_rui_hua_bar = settings.value("_show_rui_hua_bar",true).toBool();
    _show_mark_bar = settings.value("_show_mark_bar",true).toBool();
    _show_mark_manager_bar = settings.value("_show_mark_manager_bar",true).toBool();



    start_read_usb(1);
    connect(this,&PictureManager::edit_grid_indexChanged,[this]{
        _editing_picture_model->reset_to_empty(m_edit_grid_index);
        _collected_picture_model->checked_thoes_ids(QStringList()); //下面那些去掉打钩
    });
    _editing_picture_model->reset_to_empty(m_edit_grid_index);
}

void PictureManager::load_pictures(qint64 patient_id)
{
    if(_patient_id != patient_id)
    {
        reset(patient_id);
        ImagesTable * instance = ImagesTable::getInstance();
        instance->query_this_patient_picture(patient_id, _all_data);
        { //先把图片数据放到图片集合里，后面有需要再加载
            QString origin_path_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_Path_TEXT);
            QString last_state_path_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_Last_State_Path_TEXT);
            QString picture_id_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT);
            QString state_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_State_Integer);
            QString rui_hua_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Rui_Hua_integer);

            for(int i =0; i < _all_data.size() ;i++)
            {
                QJsonObject & new_picture = _all_data[i];
                ImageState state = ImageState(new_picture[state_key].toInt());
                ImageFactory::ImageData data;
                data.last = std::shared_ptr<QImage>(0);
                data.origin = std::shared_ptr<QImage>(0);
                data.state = state;
                data.origin_path = new_picture[origin_path_key].toString();
                data.last_path = new_picture[last_state_path_key].toString();
                data.Rui_Hua_integer = new_picture[rui_hua_key].toInt();
                QString picture_id = new_picture[picture_id_key].toString();
                ImageFactory::get_instance()->add_image(picture_id,data);
                if(state == ImageState::Origin)
                    Configure::get_origin_visible_propertys(new_picture);
                //  _pictures_old_visible_property[picture_id] = Picture_Visible_Property_Trail();

                // _editing_iamge_provider->addImage(new_picture[picture_id_key].toString(),data);
            }

        }
        QJsonArray temp_collected_tee_indexs;
        QJsonArray temp_collected_tee_text;
        if(_fillter_for_patient_page_collected_picture->_collected_tee_type == "kid"){
            temp_collected_tee_indexs = _fillter_for_patient_page_collected_picture->_collected_kid_teeth_indexs;
            temp_collected_tee_text = _fillter_for_patient_page_collected_picture->_collected_kid_teeth_text;
        }
        else{
            temp_collected_tee_indexs = _fillter_for_patient_page_collected_picture->_collected_adult_teeth_indexs;
            temp_collected_tee_text = _fillter_for_patient_page_collected_picture->_collected_adult_teeth_text;
        }
        //_fillter_for_patient_page_collected_picture->_collected_tee_indexs_selecter_image_provider->get_database_indexs_format(temp_collected_tee_indexs);

        for(int i =0; i < _all_data.size();i++)
        {
            const QJsonObject & info = _all_data.at(i);
            qint64 pts = info[ImagesTable::getInstance()->get_field_name(ImagesTable::Field(ImagesTable::Field::Image_ID_TEXT))].toString().toLongLong();
            if(pts > _today_pts){
                _today_picture_model->append(info);
                if(_today_picture_model->getCount() > 4)
                    _today_picture_model->remove_first();
            }
            bool ok = test_filter(info,
                                  _fillter_for_patient_page_collected_picture->collected_source_type,
                                  _fillter_for_patient_page_collected_picture->collected_level,
                                  _fillter_for_patient_page_collected_picture->collected_has_tee_type,
                                  _fillter_for_patient_page_collected_picture->collected_date_begin,
                                  _fillter_for_patient_page_collected_picture->collected_date_end,
                                  temp_collected_tee_indexs,
                                  temp_collected_tee_text,
                                  _fillter_for_patient_page_collected_picture->_collected_tee_type);
            if(ok)
                _collected_picture_model->append(info);

        }
    }
}

void PictureManager::da_task(const QString &task, const QJsonObject &info)
{
    if(task == "new_picture")
    {

    }
}

void PictureManager::mouse_event(float x, float y, const QString &action, const QString &doing)
{
    if(doing == "collecting")
    {
        //_tee_indexs_selecter_provider_for_collecting_image->mouse_event(x,y,action);
        //if(_tee_indexs_selecter_provider_for_collecting_image->dirty())
        //   collecting_picture_url_change(_tee_indexs_selecter_provider_for_collecting_image->get_url());//切换图片刷新
    }else if(doing == "collected")
    {
        //   _fillter_for_patient_page_collected_picture->_collected_tee_indexs_selecter_image_provider->mouse_event(x,y,action);
        //   if(_fillter_for_patient_page_collected_picture->_collected_tee_indexs_selecter_image_provider->dirty())
        //   {
        //      emit collected_picture_url_change(_fillter_for_patient_page_collected_picture->_collected_tee_indexs_selecter_image_provider->get_url());
        //    emit  collected_choosed_tee_indexsChanged(); //重新筛选图片
        //  }
    }
    else if(doing == "picture_info") //图片信息页面
    { //图片信息页面的牙齿选择,图片可能要刷新
        //   _tee_indexs_selecter_provider_for_picture_info->mouse_event(x,y,action);
        // if(_tee_indexs_selecter_provider_for_picture_info->dirty())
        //  {
        //            QJsonArray new_indexs;
        //            _tee_indexs_selecter_provider_for_picture_info->get_database_indexs_format(new_indexs);
        //            QString new_indexs_str;
        //            Tools::jsonObj2String(new_indexs,new_indexs_str);
        //    emit picture_info_tee_indexs_change(new_indexs_str);
        //            emit picture_info_url_change(_tee_indexs_selecter_provider_for_picture_info->get_url());
        //        }
    }
    else
    {
        Log::e(TAG,"不明的doing字段:"+doing);
    }

}

//void PictureManager::init_pircture_info_iamge_provider(const QString &tee_type, const QString &tee_indexs)
//{
//    _tee_indexs_selecter_provider_for_picture_info->reinit(tee_type,tee_indexs);
//    emit picture_info_url_change(_tee_indexs_selecter_provider_for_picture_info->get_url());
//}

//QString PictureManager::switch_tee_type_for_picture_info(const QString &new_type)
//{
//    QJsonArray indexs;
//    _tee_indexs_selecter_provider_for_picture_info->select(new_type);
//    emit picture_info_url_change(_tee_indexs_selecter_provider_for_picture_info->get_url());
//    _tee_indexs_selecter_provider_for_picture_info->get_database_indexs_format(indexs);
//    QString result;
//    Tools::jsonObj2String(indexs,result);
//    return result;
//}



void PictureManager::on_import_file(const QJsonArray &files)
{
    QJsonObject new_pic ;
    if(DEBUG)
        qDebug() <<"正在导入" << files ;
    int i =0;
    qint64 now = Tools::get_1970_ms();
    QString lastest_url;
    for(auto it = files.begin();it != files.end();it++)
    {
        QString url = it->toString().replace("file:///","");
        if(DEBUG)
            Log::d(TAG,"结果url="+url);
        const  QString result_url = Tools::create_file_name(now+i,Configure::Origin_Image_Store_Node_Dir_Name,"png");
        //QFile::copy(url,result_url);
        const   QString pillar_path  =  Tools::create_file_name(now+i,Configure::Pill_Line_Image_Store_Node_Dir_Name,"png");//柱状图
        const   QString last_state_path  =  Tools::create_file_name(now+i,Configure::Last_State_Image_Store_Node_Dir_Name,"png");//最后状态

        QImage pic(url);
        pic = pic.convertToFormat(QImage::Format_Grayscale16);
        bool ok = Core::ImageTool::save(Tools::get_root_path()+"/"+ result_url,pic);
        if(!ok)
        {
            qDebug() << "无法保存原图" << Tools::get_root_path()+"/"+ result_url;
            continue;
        }
        QImage result;
        Core::ImageTool::create_pillar_iamge(pic,result,100,Qt::green,Qt::black);
        ok = result.save(Tools::get_root_path()+"/"+ pillar_path);
        if(!ok)
        {
            qDebug() << "无法保存柱状图" <<Tools::get_root_path()+"/"+ pillar_path;
            continue;
        }

        QImage * final_pic = ImageFactory::use_default_value_create_an_final_image(pic);
        ok =  Core::ImageTool::save(Tools::get_root_path()+"/"+ last_state_path,*final_pic);
        delete final_pic;
        if(!ok)
        {
            qDebug() << "无法保存结果图" << Tools::get_root_path()+"/"+ last_state_path;
            continue;
        }


        auto temp_pic  = on_new_image(result_url,last_state_path,pillar_path,Configure::Window_Begin,Configure::Window_End,now+i,ImagesTable::PictureSoureType::Import,pic.width(),pic.height(),1.0,1.0);
        ok = !temp_pic.empty();
        if(!ok)
        {
            qDebug() << "导入图片无法存到数据库";

            QFile f(result_url);
            if(f.exists())
                f.remove();
            QFile f2(pillar_path);
            if(f2.exists())
                f2.remove();
            continue;
        }
        else
        {
            if(i == files.size() -1)
            {
                //lastest_url = result_url;
                new_pic = temp_pic;
                _lastest_picture_id =  QString::number(now+i);
            }
        }
        i++;
    }
    auto st = Settings::get_instance();
    QString ch = st->tran("导入了")+ QString("%1").arg(i) +st->tran("个图片");
    show_message(TAG,ch);
    //setCollecting_lastest_picture_url(lastest_url);
    set_last_pic_info(new_pic);
}

void PictureManager::delete_one_picuture(const QString & id,const QString &path)
{

    if(id == _lastest_picture_id)
    {
        _lastest_picture_id = "-1";
        //setCollecting_lastest_picture_url("");
        set_last_pic_info(QJsonObject());
    }
    _today_picture_model->remove(id);
    _editing_picture_model->remove(id);
    setEediting_count(_editing_picture_model->getCount());
    _collected_picture_model->remove(id);
    QString key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT);
    QString pillar_path_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_Pillar_Path_TEXT);
    QString pillar_path;
    QString last_state_path_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_Last_State_Path_TEXT);
    QString last_state_path;
    for(int i =0 ;i < _all_data.size();i ++)
    {
        const QJsonObject & ele = _all_data.at(i);
        if(ele[key].toString() == id)
        {
            pillar_path = ele[pillar_path_key].toString();
            last_state_path = ele[last_state_path_key].toString();
            _all_data.removeAt(i);
            break;
        }
    }

    ImagesTable::getInstance()->remove(id);
    LineTable::getInstance()->remove_this_picture_all_lines(id);

    QFile file(path);
    if(file.exists())
        file.remove();
    QFile pillar_file(pillar_path);
    if(pillar_file.exists())
        pillar_file.remove();
    QFile last_state_path_file(last_state_path);
    if(last_state_path_file.exists())
        last_state_path_file.remove();
    auto st = Settings::get_instance();
    show_message(TAG,st->tran("图片删除成功"));
}

void PictureManager::checked_export_or_delete(bool checked)
{
    _collected_picture_model->checked_export_or_delete( checked);
}

void PictureManager::delete_checked_pictures()
{

    std::map<QString,QString>  delete_pictures;
    _collected_picture_model->delete_checked_pictures(delete_pictures);
    QString picture_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT);
    QString last_state_path_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_Last_State_Path_TEXT);
    QString pillar_path_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_Pillar_Path_TEXT);
    QStringList * paths = new QStringList;
    for(auto it = delete_pictures.begin();it != delete_pictures.end();it++)
    {
        for(int i =0; i < _all_data.size();i++)
        {
            const QJsonObject & ele =  _all_data.at(i);
            QString temp_id = ele[picture_key].toString();
            if(temp_id == it->first)
            {
                _today_picture_model->remove(temp_id);
                _editing_picture_model->remove(temp_id);
                ImagesTable::getInstance()->remove(temp_id);
                LineTable::getInstance()->remove_this_picture_all_lines(temp_id);
                *paths << it->second;
                *paths << ele[last_state_path_key].toString();
                *paths << ele[pillar_path_key].toString();
                _all_data.removeAt(i);
                break;
            }
        }
    }
    setEediting_count(_editing_picture_model->getCount());

    if(paths->size() > 0)
    {
        std::thread t([paths,this]{
            for(auto & ele : *paths)
            {
                QFile file(ele);
                if(file.exists())
                    file.remove();
            }
            auto st = Settings::get_instance();
            show_message(TAG,QString(st->tran("删除") + "%1" + st->tran("个图片成功")).arg(paths->size()/3));
            delete paths;
        });
        t.detach();
    }
    else
    {
        auto st = Settings::get_instance();
        show_message(TAG,st->tran("请先勾选要删除的图片"));
        delete paths;
    }
}

void PictureManager::export_checked_pictures(const QString & des_dir)
{
    QStringList * paths = new QStringList;
    QString temp = des_dir;
    QString result = temp.replace("file:///","");
    if(result.endsWith("/"))
        result = result.mid(0,result.length()-1);

    _collected_picture_model->get_checked_files(*paths);

    QString origin_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_State_Integer);
    QString fake_color_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Enable_Fake_Color_Filter_BOOLEAN);
    QString rotate_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Rotate_integer);
    QJsonArray temp2;
    for(int i =0; i < paths->size(); i++)
    {
        QString id = paths->at(i);
        QJsonObject params = get_params(id);
        ImageState state = ImageState(params[origin_key].toInt());
        int rotate = params[rotate_key].toInt();
        QString result_color;
        if(state == ImageState::Origin)
        {
            result_color = "16gray";
        }
        else {
            bool fake_color = params[fake_color_key].toBool();
            if(fake_color == false)
                result_color = "16gray";
            else
                result_color = "rgba";
        }
        QJsonObject info;
        info["rotate"] = rotate;
        info["picture_id"] = id;
        info["path"] = result+"/" +id+".png";
        info["color"] = result_color;
        info["task"] = "export_checked_pictures";
        temp2.append(info);
    }

    delete paths;
    begin_export_pictures(temp2);
    return; //新模式

    //    if(DEBUG)
    //        qDebug() <<"开始导出" << result << "*paths" <<*paths;
    //    if(paths->size() > 0)
    //    {
    //        std::thread t([paths,result,this]{
    //            for(const auto &  path : *paths)
    //            {
    //                QString url = result+"/" +path+".png";
    //                QImage out;
    //                bool ok =  get_final_image(path,out);
    //                if(ok)
    //                {
    //                    ok=   out.save(url,"png",100);
    //                    if(!ok)
    //                        show_message(TAG,"导出失败: 图片保存失败");
    //                }
    //                else
    //                {
    //                    show_message(TAG,"导出失败: 图片没找到");
    //                    break;
    //                }
    //                //                QFile file(path);
    //                //                if(file.exists())
    //                //                {
    //                //                    QStringList dir_node = path.split("/");
    //                //                    QFile::copy(path,result+"/"+dir_node.at(dir_node.size()-1));
    //                //                }
    //                //                else
    //                //                {
    //                //                    Log::e(TAG,"文件不存在:"+path);
    //                //                }
    //            }
    //            show_message(TAG,"导出完成");
    //            delete paths;
    //        });
    //        t.detach();
    //    }
    //    else
    //    {
    //        delete paths;
    //    }
}

void PictureManager::export_one_picture(const QString &picture_id, const QString &target)
{
    QString temp = target;
    temp = temp.replace("file:///","");



    QString origin_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_State_Integer);
    QString fake_color_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Enable_Fake_Color_Filter_BOOLEAN);
    QString rotate_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Rotate_integer);
    QJsonArray temp2;

    QJsonObject params = get_params(picture_id);
    ImageState state = ImageState(params[origin_key].toInt());
    int rotate = params[rotate_key].toInt();
    QString result_color;
    if(state == ImageState::Origin)
    {
        result_color = "16gray";
    }
    else {
        bool fake_color = params[fake_color_key].toBool();
        if(fake_color == false)
            result_color = "16gray";
        else
            result_color = "rgba";
    }
    QJsonObject info;
    info["rotate"] = rotate;
    info["picture_id"] = picture_id;
    info["path"] = temp;
    info["color"] = result_color;
    info["task"] = "export_one_picture";
    temp2.append(info);



    begin_export_pictures(temp2);
    return;
    //    QImage out;
    //    bool ok =  get_final_image(picture_id,out);
    //    if(!ok)
    //    {
    //        show_message(TAG,"导出失败: 图片没找到");
    //        return ;
    //    }
    //    ok = out.save(temp,"png",100);
    //    if(ok)
    //        show_message(TAG,"导出完成");
    //    else
    //        show_message(TAG,"导出失败");


}

void PictureManager::update_picture_from_picture_info_page(const QString & picture_id,const QJsonObject &new_property)
{
    auto st = Settings::get_instance();
    bool ok = _update_picture_property_any_where(picture_id, new_property, true,true);
    if(ok)
        show_message(TAG,st->tran("成功"));
    else
        show_message(TAG,st->tran("失败") + picture_id);
}

//void PictureManager::update_picture_property_any_where(const QString &picture_id, const QJsonObject &new_property)
//{
//    _update_picture_property_any_where(picture_id, new_property, false,true);
//}

bool PictureManager::_update_picture_property_any_where(const QString &picture_id, const QJsonObject &new_property, bool test_filter_,bool save_to_datebase)
{
    //qDebug() << new_property;
    //    let dirty = _new_info.Describe_TEXT !== _old_info.Describe_TEXT || _new_info.Tee_Type_TEXT !== _old_info.Tee_Type_TEXT ||_new_info.Tee_Indexs_TEXT  !== _old_info.Tee_Indexs_TEXT  || _new_info.Level_integer !== _old_info.Level_integer
    QString picture_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT);
    const QJsonObject * picture = nullptr;
    for(int i = 0;i < _all_data.size();i++)
    {
        QJsonObject & ele = _all_data[i];
        if(ele[picture_key].toString() == picture_id)
        {
            picture = &ele;
            for(auto it = new_property.begin(); it != new_property.end();it++)
            {
                ele[it.key()] = it.value();
            }
            break;
        }
    }

    if(picture == nullptr)
    {
        Log::e(TAG,"代码逻辑错误,无法找到图片id:"+picture_id);
        return false;
    }
    else
    {
        _collected_picture_model->update_property(picture_id,new_property);
        //  _today_picture_model->update_property(picture_id,new_property);
        _editing_picture_model->update_property(picture_id,new_property);
        if(test_filter_)
        {
            QJsonArray collected_indexs;
            QJsonArray collected_text;
            if(_fillter_for_patient_page_collected_picture->_collected_tee_type == "kid"){
                collected_indexs = _fillter_for_patient_page_collected_picture->_collected_kid_teeth_indexs;
                collected_text = _fillter_for_patient_page_collected_picture->_collected_kid_teeth_text;
            }
            else{
                collected_indexs = _fillter_for_patient_page_collected_picture->_collected_adult_teeth_indexs;
                collected_text = _fillter_for_patient_page_collected_picture->_collected_adult_teeth_text;
            }

            //  _fillter_for_patient_page_collected_picture->_collected_tee_indexs_selecter_image_provider->get_database_indexs_format(collected_indexs);
            bool ok = test_filter(*picture,//更新属性不会再更新图片url
                                  _fillter_for_patient_page_collected_picture->collected_source_type,
                                  _fillter_for_patient_page_collected_picture->collected_level,
                                  _fillter_for_patient_page_collected_picture->collected_has_tee_type,
                                  _fillter_for_patient_page_collected_picture->collected_date_begin,
                                  _fillter_for_patient_page_collected_picture->collected_date_end,
                                  collected_indexs,
                                  collected_text,
                                  _fillter_for_patient_page_collected_picture->_collected_tee_type);

            if(ok)
            {
                _collected_picture_model->append(*picture);
                QString checked_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Checked_For_Add_To_Paint_List_Bool);
                if((*picture)[checked_key].toBool())
                {
                    _editing_picture_model->append(*picture);
                    _collected_picture_model->checked_thoes_ids(_editing_picture_model->get_checked_ids());

                }
            }
            else
            {
                QString picture_id = (*picture)[picture_key].toString();
                _collected_picture_model->remove(picture_id);
                _editing_picture_model->remove(picture_id);
            }
            setEediting_count(_editing_picture_model->getCount());
        }
        if(save_to_datebase)
        {
            auto table = ImagesTable::getInstance();
            table->update_from_picture_info_page_param(picture_id,new_property);
        }
        return true;
    }
}

//void PictureManager::update_property_to_data_base(const QString &picture_id, const QJsonObject &new_property)
//{
//    ImagesTable::getInstance()->update_from_picture_info_page_param(picture_id,new_property);
//}

void PictureManager::update_one_visible_value(const QString & picture_id, const QString &key, const QJsonValue &value)
{
    QJsonObject new_property;
    new_property[key] = value;
    if(_current_image_state == ImageState::Origin)
    {
        _change_state_property = new_property;
        setCurrent_editing_picture_state(int(ImageState::Last));
        qDebug() << "现在是原图状态,但有值改变，现在改成所有图片为原图+一个新值";
        //_current_image_state  = ImageState::Last;
        //Configure::get_origin_visible_propertys(new_property);
    }
    else
    {
        _change_state_property = QJsonObject();
        _update_picture_property_any_where(picture_id,new_property,false,true);
    }
}

void PictureManager::update_mm_per_pixel_value(const QString &picture_id, const QString &key, const QJsonValue &value)
{
    QJsonObject new_property;
    new_property[key] = value;
    _update_picture_property_any_where(picture_id,new_property,false,true);
}

void PictureManager::update_scale_factor_to_model(const QString & picture_id, float factor)
{
    QString scale_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Scale_float);
    QJsonObject obj;
    obj[scale_key] = factor;
    only_update_property_to_model(picture_id, obj);
}

void PictureManager::only_update_property_to_model(const QString &picture_id, const QJsonObject &new_property)
{
    _update_picture_property_any_where(picture_id, new_property, false,false);
}

void PictureManager::add_one_picture_to_editing_picture_model(const QString &picture_id)
{
    float sc_fs[5] = {0.25,0.25,0.25,0.2,0.15};
    QString picture_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT);
    for(int i =0; i < _all_data.size();i++)
    {
        const QJsonObject & ele =_all_data.at(i);
        if(ele[picture_key].toString() == picture_id)
        {
            QJsonObject temp = ele;
            temp["Scale_float"] = sc_fs[m_edit_grid_index];
            _editing_picture_model->append(temp);
            _collected_picture_model->checked_thoes_ids(_editing_picture_model->get_checked_ids());
            setEediting_count(_editing_picture_model->getCount());
            _collected_picture_model->set_one_checked_for_on_editing(picture_id,true);
            break;
        }
    }
}

void PictureManager::add_checked_pictures_to_editing_picture_model()
{
    QString checked_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Checked_For_Export_Or_Delete_Bool);
    QString checked_key_for_paint_list = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Checked_For_Add_To_Paint_List_Bool);
    QString picture_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT);
    const QList<QJsonObject> & data = _collected_picture_model->get_data();
    for(int i =0; i < data.size();i++)
    {
        const QJsonObject & ele =data.at(i);
        if(ele[checked_key].toBool())
        {

            _editing_picture_model->append(ele);
            _collected_picture_model->checked_thoes_ids(_editing_picture_model->get_checked_ids());

            if(ele[checked_key_for_paint_list].toBool() == false)
                _collected_picture_model->set_one_checked_for_on_editing(i,true);
        }
    }
    setEediting_count(_editing_picture_model->getCount());
}

void PictureManager::remove_one_picture_on_editing_picture_model(const QString &picture_id)
{
    _editing_picture_model->remove(picture_id);
    setEediting_count(_editing_picture_model->getCount());
    _collected_picture_model->set_one_checked_for_on_editing(picture_id,false);
}

void PictureManager::remove_all_picture_on_editing_picture_model()
{
    _editing_picture_model->reset_to_empty(m_edit_grid_index);
    setEediting_count(_editing_picture_model->getCount());
    _collected_picture_model->set_all_checked_for_on_editing(false);
}

QString PictureManager::get_lastest_picture_id()
{
    return  _lastest_picture_id;
}

QJsonObject PictureManager::get_params(const QString &picture_id)
{

    float sc_fs[5] = {0.25,0.25,0.25,0.2,0.15};




    QString picture_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT);
    for(int i =0;i <_all_data.size();i++)
    {
        const QJsonObject & ele = _all_data.at(i);
        if(ele[picture_key] == picture_id)
        {
            QJsonObject temp = ele;
            temp["Scale_float"] = sc_fs[m_edit_grid_index];
            return temp;
        }
    }
    Log::e(TAG,"代码逻辑错误,无法找到id:"+picture_id);
    return QJsonObject();
}

void PictureManager::apply_filter(const QString &picture_id, int filter)
{ //锐化参数，窗位和窗宽都会被重置,且最后要通知ui，做同步
    bool ok =  ImageFactory::get_instance()->apply_filter(picture_id,ImageFilter(filter));

    // QImage * temp = ImageFactor::filter(ImageFilter(filter),*ptr);
    // ImageFactor::get_instance()->set_image(picture_id,std::shared_ptr<QImage>(temp),ImageState::Last);
    //_editing_iamge_provider->setImage(picture_id,std::shared_ptr<QImage>(temp),ImageState::Last);
    if(ok)
    {
        if(_current_image_state == ImageState::Origin)
        {
            _change_filter_then_update_iamge_state  = true;
            setCurrent_editing_picture_state(int(ImageState::Last)); //通知界面的状态同步
        }

        {
            QJsonObject new_property;
            Configure::get_filtered_origin_visible_propertys(new_property,ImageFilter(filter));
            QString filter_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Filter_Index_integer);
            new_property[filter_key] = filter;
            _update_picture_property_any_where(picture_id ,new_property,false,true);
            emit aync_apply_filter_to_eidting_page_right_tool_bar(new_property);

            new_property = QJsonObject();
            QString url_value = "image://editing_images/"+ picture_id+ "/"+QString::number(Tools::get_1970_ms());
            QString url_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Url_str);
            new_property[url_key] = url_value;
            only_update_property_to_model(picture_id,new_property);
            // only_update_property_to_model(picture_id,new_property);
            //   QString rui_hua_key = tb->get_field_name(ImagesTable::Field::Rui_Hua_integer);
            //  QString window_begin_key = tb->get_field_name(ImagesTable::Field::Window_Begin_integer);
            //  QString window_end_key = tb->get_field_name(ImagesTable::Field::Window_End_integer);
            //  QString url_key = tb->get_field_name(ImagesTable::Field::Url_str);

            //        QString brightness_key = tb->get_field_name(ImagesTable::Field::Luminance_float);
            //        QString const_key = tb->get_field_name(ImagesTable::Field::Contrast_float);
            //        QString gama_key = tb->get_field_name(ImagesTable::Field::Gama_float);
            //        QString rotate_key = tb->get_field_name(ImagesTable::Field::Rotate_integer);
            //        QString hor_mir_key = tb->get_field_name(ImagesTable::Field::Enable_Hor_Mirror_BOOLEAN);
            //        QString hor_hor_key = tb->get_field_name(ImagesTable::Field::Enable_Hor_Mirror_BOOLEAN);
            //        new_property[ ]
            //        new_property[rui_hua_key]  = 0.0;
            //        new_property[window_begin_key] = 0;
            //        new_property[window_end_key] = 65535;

            //new_property[url_key] = "image://editing_images/"+ picture_id+"/"+ QString::number(Tools::get_1970_ms());
            //QJsonObject new_url;
            // new_url[url_key] = "image://editing_images/"+ picture_id+"/"+ QString::number(Tools::get_1970_ms());
            // only_update_property_to_model(picture_id,new_url);
            // update_picture_property_any_where(picture_id ,new_property);
            //  emit aync_new_value_to_eidting_page_right_tool_bar(new_property); //ui,更新值
            //        auto it = _pictures_old_visible_property.find(picture_id);
            //        if(it != _pictures_old_visible_property.end())
            //        {
            //            new_property = QJsonObject();
            //            new_property[filter_key] = filter;
            //            it->second._queue.push_back(new_property);
            //        }
        }
    }else
    {
        Log::e(TAG,"图片工厂里面找不到图片id,已经取消滤镜:"+picture_id);
    }
}
static float get_area(float x1, float y1, float x2, float y2, float x3, float y3)
{
    return fabs(x1*y2+y1*x3+x2*y3-y2*x3-y3*x1-y1*x2)/2.0f;
}
static bool point_int_triangle(float x, float y, float x1, float y1, float x2, float y2, float x3, float y3)
{
    float s  = get_area(x1, y1, x2, y2, x3, y3);
    float s1 = get_area(x, y, x2, y2, x3, y3);
    float s2 = get_area(x1, y1, x, y, x3, y3);
    float s3 = get_area(x1, y1, x2, y2, x, y);

    return  fabs(s - s1 - s2 - s3) < 0.0001;
}
static QImage * no_scale(int w,int h,QByteArray & buf)
{

    typedef  quint16 Deepth;
    Deepth * result_buf = new Deepth[w * h];
    quint16 * buf_ct_value = new quint16[w * h];
    QByteArray & data  = buf;
    quint16 * ptr = (quint16*)data.data();
    for(int i =0; i < w*h;i++)
    {
        quint16 ct_value = ptr[i];
        buf_ct_value[i] = 16*ct_value;
    }

    float x1 = 0, y1 = 1970,x2 = 0,y2 = 2280,x3 = 300,y3 = 2280;
    float x11 = 1360,y11 = 2280, x22 = 1660, y22 = 2280, x33 = 1660,y33 = 1970;
    for(int y =0; y < h;y++)
    {
        quint16 * current_row =  buf_ct_value + (y*w);
        for(int x = 0; x < w;x++)
        {
            Deepth   resultPixel =65535 -  current_row[x];

            if(y > 1970)
            {
                bool in1 = point_int_triangle(x,y,x1, y1, x2, y2, x3, y3);
                bool in2 = point_int_triangle(x,y,x11, y11, x22, y22, x33, y33);
                if(in1 || in2)
                    resultPixel = 65535;
            }

            result_buf[y*w+x]  = resultPixel;
        }
    }

    delete [] buf_ct_value;
    QImage no_clip_pic((unsigned char *)result_buf,w,2276,QImage::Format::Format_Grayscale16);
    return   new QImage(no_clip_pic);
}

static QImage * ct_vaule_to_image_usb_2(int w, int h, QByteArray &buf)
{
    quint16 min =65335,max =0;
    typedef  quint16 Deepth;
    Deepth * result_buf = new Deepth[w * h];
    quint16 * buf_ct_value = new quint16[w * h];
    QByteArray & data  = buf;
    quint16 * ptr = (quint16*)data.data();


    memcpy(buf_ct_value,ptr,w*h*sizeof(quint16));

    float x1 = 0, y1 = 1970,x2 = 0,y2 = 2280,x3 = 300,y3 = 2280;
    float x11 = 1360,y11 = 2280, x22 = 1660, y22 = 2280, x33 = 1660,y33 = 1970;


    for(int y =0; y < h;y++)
    {
        quint16 * current_row = ptr + (y*w);
        for(int x = 0; x < w;x++)
        {
            qint16 ct_value = current_row[x];
            bool in1 = point_int_triangle(x,y,x1, y1, x2, y2, x3, y3);
            bool in2 = point_int_triangle(x,y,x11, y11, x22, y22, x33, y33);
            if(in1 || in2)
            {

            }
            else
            {
                if (ct_value < min)
                    min = ct_value;
                if (ct_value > max)
                    max = ct_value;
            }
        }
    }

    float dis = max - min;
    qDebug() << "max" << max << "min" << min <<"dis" << dis;

    for(int y =0; y < h;y++)
    {
        quint16 * current_row =  buf_ct_value + (y*w);
        for(int x = 0; x < w;x++)
        {
            Deepth resultPixel;         // 映射到8bit的像素
            Deepth tempPixel = float(current_row[x] - min)/dis*65535.0;// *255.0;
            if ( tempPixel > 65535 )
                resultPixel = 65535;
            else if ( tempPixel < 0 )
                resultPixel = 0;
            else
                resultPixel =  tempPixel;
            resultPixel =65535 - resultPixel;

            if(y > 1970)
            {
                bool in1 = point_int_triangle(x,y,x1, y1, x2, y2, x3, y3);
                bool in2 = point_int_triangle(x,y,x11, y11, x22, y22, x33, y33);
                if(in1 || in2)
                    resultPixel = 65535;
            }

            result_buf[y*w+x]  = resultPixel;
        }
    }

    delete [] buf_ct_value;
    QImage no_clip_pic((unsigned char *)result_buf,w,2276,QImage::Format::Format_Grayscale16);
    return   new QImage(no_clip_pic);
}

static QImage * ct_vaule_to_image_usb_3(int w, int h, QByteArray &buf)
{
    double min =65335,max =0;
    typedef  quint16 Deepth;
    Deepth * result_buf = new Deepth[w * h];
    quint16 * buf_ct_value = new quint16[w * h];
    QByteArray & data  = buf;
    quint16 * ptr = (quint16*)data.data();


    quint32 *counts = new quint32[4096];
    memset(counts,0,4096*sizeof(quint32));
    memcpy(buf_ct_value,ptr,w*h*sizeof(quint16));

    float x1 = 0, y1 = 1970,x2 = 0,y2 = 2280,x3 = 310,y3 = 2280;
    float x11 = 1350,y11 = 2280, x22 = 1660, y22 = 2280, x33 = 1660,y33 = 1970;

    int sum_ele = 0;
    for(int y =0; y < h;y++)
    {
        quint16 * current_row = ptr + (y*w);
        for(int x = 0; x < w;x++)
        {
            qint16 ct_value = current_row[x];
            bool in1 = point_int_triangle(x,y,x1, y1, x2, y2, x3, y3);
            bool in2 = point_int_triangle(x,y,x11, y11, x22, y22, x33, y33);

            if(in1 || in2 || y >= 2270)
            {

            }
            else
            {
                if(ct_value >4095)
                    ct_value = 4095;
                counts[ct_value]++;
                sum_ele++;
            }
        }
    }

    {
        int min_ele_count = 0;
        for(int ct_value=0; ct_value < 4096;ct_value++ )
        {
            min_ele_count += counts[ct_value];
            double factor = double(min_ele_count)/double(sum_ele);
            if(factor > 0.05/100.0)
            {
                int min_ct_value = ct_value-1;
                if(min_ct_value < 0)
                    min_ct_value = 0;
                min = min_ct_value;
                break;
            }
        }
    }

    {
        int max_ele_count = 0;
        for(int ct_value=4095; ct_value >= 0 ;ct_value-- )
        {
            max_ele_count += counts[ct_value];
            double factor = double(max_ele_count)/double(sum_ele);
            if(factor > 0.05/100.0)
            {
                int max_ct_value = ct_value+1;
                if(max_ct_value > 4095)
                    max_ct_value = 4095;
                max = max_ct_value;
                break;
            }//
        }
    }




    double dis = max - min;
    qDebug() << "max" << max << "min" << min <<"dis" << dis;

    for(int y =0; y < h;y++)
    {
        quint16 * current_row =  buf_ct_value + (y*w);
        for(int x = 0; x < w;x++)
        {
            double resultPixel;         // 映射到8bit的像素
            double tempPixel = double(double(current_row[x]) - min)/dis*65535.0;// *255.0;
            if ( tempPixel > 65535 )
                resultPixel = 65535;
            else if ( tempPixel < 0 )
                resultPixel = 0;
            else
                resultPixel =  tempPixel;
            resultPixel =65535.0 - resultPixel;

            //            if(y > 1970)
            //            {
            bool in1 = point_int_triangle(x,y,x1, y1, x2, y2, x3, y3);
            bool in2 = point_int_triangle(x,y,x11, y11, x22, y22, x33, y33);
            if(in1 || in2)
                resultPixel = 65535;
            //    }

            result_buf[y*w+x]  = resultPixel;
        }
    }

    delete [] buf_ct_value;
    QImage no_clip_pic((unsigned char *)result_buf,w,2276,QImage::Format::Format_Grayscale16);
    delete [] counts;
    return   new QImage(no_clip_pic);
}

void fixed2(QByteArray &need_fix, const QByteArray &dark, const QByteArray &light)
{



    float x1 = 0, y1 = 1970,x2 = 0,y2 = 2280,x3 = 310,y3 = 2280;
    float x11 = 1350,y11 = 2280, x22 = 1660, y22 = 2280, x33 = 1660,y33 = 1970;


    quint16 * need_fix_ptr = (quint16 *)need_fix.data();
    const quint16 * dark_ptr =(const quint16 *) dark.data();
    const quint16 * light_ptr =(const quint16 *) light.data();
    int w = 1660;
    int h =2280;
    double dark_sum=0,light_sum = 0;
    int count_valid_px = 0;

    for(int y =0; y < h;y++)
    {
        const  quint16 * current_row_dark = dark_ptr + (y*w);
        const quint16 * current_row_light = light_ptr + (y*w);
        for(int x = 0; x < w;x++)
        {
            bool in1 = point_int_triangle(x,y,x1, y1, x2, y2, x3, y3);
            bool in2 = point_int_triangle(x,y,x11, y11, x22, y22, x33, y33);

            if(in1 || in2 || y >= 2270)
            {
            }
            else
            {
                dark_sum += current_row_dark[x];
                light_sum += current_row_light[x];
                count_valid_px++;
            }
        }
    }


    double average_dark = dark_sum/double(count_valid_px); //平均暗
    double average_light = light_sum/double(count_valid_px);//平均梁


    for(int y =0; y < h;y++)
    {
        const  quint16 * current_row_dark = dark_ptr + (y*w);
        const quint16 * current_row_light = light_ptr + (y*w);
        quint16 * need_fixed_row = need_fix_ptr + (y*w);
        for(int x = 0; x < w;x++)
        {
            bool in1 = point_int_triangle(x,y,x1, y1, x2, y2, x3, y3);
            bool in2 = point_int_triangle(x,y,x11, y11, x22, y22, x33, y33);

            if(in1 || in2 || y >= 2270)
            {
               need_fixed_row[x] = quint16(0);
            }
            else
            {
                double dark_px  = current_row_dark[x];
                double light_px = current_row_light[x];
                double px = need_fixed_row[x];//当前像素
                double ret = (double(average_light-average_dark)/double(light_px-dark_px))*double(px-dark_px)+average_dark;
                if(ret < 0)
                    ret = 0;
                if(ret > 4095)
                    ret = 4095;
                need_fixed_row[x] = quint16(ret);
            }
        }
    }

}


bool PictureManager::start_read_usb(int fillter)
{

    connect(_usb_reader,&UsbReader::error,this,[this]{
        //  _usb_reader->stop();
        //  clear_usb_reader(_usb_reader);
        //   _usb_reader = nullptr;
        emit not_found_usb();
        set_usb_connected(false);
    });
    connect(_usb_reader,&UsbReader::timeout,this,[this]{
        //  _usb_reader->stop();
        //  clear_usb_reader(_usb_reader);
        //   _usb_reader = nullptr;
        emit usb_timeout();
        set_usb_connected(false);
    });


    connect(_usb_reader,&UsbReader::connected,this,[this]{
        set_usb_connected(true);
    });

    connect(_usb_reader,&UsbReader::got_picture,this,[this](QByteArray * data){
        static QByteArray temp_data;
        if(data == nullptr)
        {
            if(temp_data.isNull())
                return;
            data = new QByteArray();
            *data = temp_data;
        }
        else
        {
            temp_data = *data;
        }


        qint64 now = Tools::get_1970_ms();
        const QString raw_path  =  Tools::create_file_name(now,Configure::Raw_Image_Store_Node_Dir_Name,"raw");
        QFile f(raw_path);
        f.open(QIODevice::WriteOnly);
        f.write(*data);
        f.close();


        QByteArray dark = load_dark_image();
        QByteArray light = load_light_image();


        bool ok2 = dark.size() == light.size() && light.size() == 1660*2280*2;
        auto st = Settings::get_instance();
        QString other;
        if(true)//st->get_usb_enable_fixed_bool())  //m_enable_fixed)
        {

            if(ok2)
            {
                if(m_fixed_new_version)
                    fixed2(*data,dark,light);
                else
                    Tools::fixed(*data,dark,light);

                QFile f("FFCed.raw");
                f.open(QIODevice::WriteOnly);
                f.write(*data);
                f.close();
                other= st->tran("已校准");
                // show_message("",st->tran("图片校准完成"));
            }
            else
                other = st->tran("这个传感器还未校准，请先校准再拍照");
            //show_message("",st->tran("这个传感器还未校准，请先校准"));
        }

        QImage * pic ;

        if(m_enable_scale_0_65535)
            pic =  ct_vaule_to_image_usb_3(1660,2280,*data); //会去掉最后4行
        else
            pic =no_scale(1660,2280,*data);


        if(st->get_usb_enable_denoise_bool())
        {
            QImage out;
            Core::ImageTool::image_level_up(*pic,out);
            *pic = out;
        }



        const QString in_database_path =    Tools::create_file_name(now,Configure::Origin_Image_Store_Node_Dir_Name,"png"); //原图
        const QString last_state_path  =  Tools::create_file_name(now,Configure::Last_State_Image_Store_Node_Dir_Name,"png"); //最后状态
        const QString pillar_path  =  Tools::create_file_name(now,Configure::Pill_Line_Image_Store_Node_Dir_Name,"png");//柱状图




        delete data;
        Core::ImageTool::save(Tools::get_root_path()+"/"+ in_database_path,*pic);
        QImage * final_pic = ImageFactory::use_default_value_create_an_final_image(*pic);
        Core::ImageTool::save(Tools::get_root_path()+"/"+ last_state_path,*final_pic);
        delete final_pic;
        QImage result;
        Core::ImageTool::create_pillar_iamge(*pic,result,100,Qt::green,Qt::black);
        result.save(Tools::get_root_path()+"/"+ pillar_path);

        //[ my_tr(lange_index,"Scaner"),my_tr(lange_index,"Import"),my_tr(lange_index,"Sensor")] //扫描仪,导入,传感器
        QJsonObject new_pic = on_new_image(in_database_path,last_state_path,pillar_path,Configure::Window_Begin,Configure::Window_End,now,ImagesTable::PictureSoureType::Sensor,pic->width(),pic->height(),13.5f/943.0f,26.0f/1810.f);
        //setCollecting_lastest_picture_url(in_database_path);
        set_last_pic_info(new_pic);
        setLastest_picture_id(QString::number(now));
        delete pic;
        // _usb_reader->stop();
        //  clear_usb_reader(_usb_reader);
        //   _usb_reader = nullptr;
        emit got_usb_picture();
        show_message(TAG,st->tran("传感器曝光完成")+":"+other);
    });
    _usb_reader->begin_cap(UsbReader::Mode1::VTC,UsbReader::Bin2::NoBining,UsbReader::Filter3(fillter));
    return  true;
}

bool PictureManager::stop_read_usb()
{
    if(_usb_reader == nullptr)
        return false;
    _usb_reader->stop();
    clear_usb_reader(_usb_reader);

    _usb_reader = nullptr;
    return true;
}

void PictureManager::use_lastest_usb_image()
{
    _usb_reader->got_picture(nullptr);
}

bool PictureManager::on_image_state_changed()//const QString &id, int new_state)
{ //可见属性只会更新model，不更新数据库
    if(_current_editing_picture_id == "-1")
        return false;
    //_current_image_state = ImageState(new_state);
    bool ok = ImageFactory::get_instance()->set_image_state(_current_editing_picture_id ,_current_image_state);
    if(!ok)
    {
        Log::e(TAG,"图片工厂找不到图片id,无法切换状态："+_current_editing_picture_id);
    }
    //    auto it = _all_data.begin();
    //    for(;it != _all_data.end();it++)
    //    {
    //        if(it->find(id) != it->end())
    //        {
    //            break;
    //        }
    //    }
    //    if(it == _all_data.end())
    //    {
    //        Log::e(TAG,"所有图片里面找不到图片id,无法切换状态："+id);
    //        return false;
    //    }
    //    auto trail_it =  _pictures_old_visible_property.find(id);
    //    if(trail_it == _pictures_old_visible_property.end())
    //    {
    //        Log::e(TAG,"所有轨迹里面找不到图片id,无法切换状态："+id);
    //        return false;
    //    }
    if(_current_image_state == ImageState::Origin) //原图
    {
        QJsonObject new_property;
        Configure::get_origin_visible_propertys(new_property);
        aync_new_value_to_eidting_page_right_tool_bar(new_property);
        QString url_value = "image://editing_images/"+ _current_editing_picture_id+ "/"+QString::number(Tools::get_1970_ms());
        QString url_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Url_str);
        new_property[url_key] = url_value;
        only_update_property_to_model(_current_editing_picture_id,new_property);
    }
    else //选择最后图,或者从原图改一个属性后，变成最后状态
    {
        if(_change_filter_then_update_iamge_state == true)
        {//滤镜导致状态改变
            _change_filter_then_update_iamge_state  =false;
        }
        else if(_change_state_property.empty()) //用户切换状态
        {
            QJsonObject old_property;
            ImagesTable::getInstance()->query_picture(_patient_id, _current_editing_picture_id,old_property);
            if(old_property.isEmpty())
            {
                Log::e(TAG,"数据库里面找不到图片id,无法切换状态："+_current_editing_picture_id);
                return false;
            }
            QJsonObject new_property;
            Configure::get_visible_propertys(new_property,old_property);
            aync_new_value_to_eidting_page_right_tool_bar(new_property);
            QString url_value = "image://editing_images/"+ _current_editing_picture_id+ "/"+QString::number(Tools::get_1970_ms());
            QString url_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Url_str);
            new_property[url_key] = url_value;
            only_update_property_to_model(_current_editing_picture_id,new_property);
        }
        else //用户点击滑块导致状态改变
        {
            ImageFilter  fillter;
            QString fillter_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Filter_Index_integer);
            {
                QJsonObject new_property;
                Configure::get_origin_visible_propertys(new_property);
                for(auto it = _change_state_property.begin(); it != _change_state_property.end();it++)
                {
                    new_property[it.key()] = it.value();
                }
                fillter = ImageFilter (new_property[fillter_key].toInt());
                bool ok =  ImageFactory::get_instance()->apply_filter(_current_editing_picture_id,fillter);
                _change_state_property = QJsonObject();
                _update_picture_property_any_where(_current_editing_picture_id,new_property,false,true);
                aync_new_value_to_eidting_page_right_tool_bar(new_property);
            }

            QJsonObject new_property;
            QString url_value = "image://editing_images/"+ _current_editing_picture_id+ "/"+QString::number(Tools::get_1970_ms());
            QString url_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Url_str);
            new_property[url_key] = url_value;
            only_update_property_to_model(_current_editing_picture_id,new_property);
        }
    }
    QString state_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_State_Integer);
    QJsonObject new_property;
    new_property[state_key] = int(_current_image_state); //需要更新到数据库
    _update_picture_property_any_where(_current_editing_picture_id, new_property,false,true);
    Log::i(TAG,"切换图片状态成功");
    return  true;
}

void PictureManager::on_usm_value_change(const QString &id, int value)
{
    bool ok = ImageFactory::get_instance()->set_image_rui_hua_value(id,value);
    if(ok)
    {
        QJsonObject new_property;
        QString rui_hua_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Rui_Hua_integer);
        new_property[rui_hua_key] = value;
        if(_current_image_state == ImageState::Origin)
        {
            _change_state_property = new_property;
            setCurrent_editing_picture_state(int(ImageState::Last));
        }
        else
        {
            _change_state_property = QJsonObject();
            QString url_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Url_str);
            _update_picture_property_any_where(id,new_property,false,true);
            new_property = QJsonObject();
            new_property[url_key] = "image://editing_images/"+ id+ "/"+QString::number(Tools::get_1970_ms());
            only_update_property_to_model(id, new_property);
        }
    }
    // _rui_hua_thread->add_task(id,value);
    //    QJsonObject new_property;
    //    QString rui_hua_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Rui_Hua_integer);
    //    QString url_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Url_str);
    //    new_property[rui_hua_key] = value;
    //    _update_picture_property_any_where(id,new_property,false,true);

    //    if(ok)
    //    {
    //        new_property = QJsonObject();
    //        new_property[url_key] = "image://editing_images/"+ id+ "/"+QString::number(Tools::get_1970_ms());
    //        only_update_property_to_model(id, new_property);
    //    }
}

QJsonObject PictureManager::undo()
{
    if(_current_visible_property_index > 0 && _current_visible_property_index < _picture_old_visible_property.size()+1)
        _current_visible_property_index--;
    else
    {
        setCan_undo(false);
        return QJsonObject();
    }
    Picture_Visible_Propertys temp = _picture_old_visible_property.at(_current_visible_property_index);
    _update_picture_property_any_where(_current_editing_picture_id,temp._current_property,false,true);
    bool ok =  ImageFactory::get_instance()->synchronized_image(_current_editing_picture_id,temp._current_property);
    if(ok)
    {
        QString url_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Url_str);
        QJsonObject new_property = QJsonObject();
        new_property[url_key] = "image://editing_images/"+ _current_editing_picture_id+ "/"+QString::number(Tools::get_1970_ms());
        only_update_property_to_model(_current_editing_picture_id, new_property);
        if(DEBUG)
            Log::d(TAG,"撤销参数还原成功");
        QString state_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_State_Integer);
        ImageState old_state = ImageState(temp._current_property[state_key].toInt());
        if(_current_visible_property_index == 0)
            setCan_undo(false);
        setCan_redo(true);
        if(old_state != _current_image_state)
        {
            setCurrent_editing_picture_state(int(old_state));
            return QJsonObject();
        }
        else
        {
            return  temp._current_property;
        }
    }
    else
    {
        Log::e(TAG,"撤销参数还原失败");
        setCan_undo(false);
        return QJsonObject();
    }
}

QJsonObject PictureManager::redo()
{

    if(_current_visible_property_index >= 0 && _current_visible_property_index < _picture_old_visible_property.size() -1)
        _current_visible_property_index += 1;
    else
    {
        setCan_redo(false);
        return QJsonObject();
    }
    Picture_Visible_Propertys temp = _picture_old_visible_property.at(_current_visible_property_index);
    _update_picture_property_any_where(_current_editing_picture_id,temp._current_property,false,true);
    bool ok =  ImageFactory::get_instance()->synchronized_image(_current_editing_picture_id,temp._current_property);
    if(ok)
    {
        QString url_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Url_str);
        QJsonObject new_property = QJsonObject();
        new_property[url_key] = "image://editing_images/"+ _current_editing_picture_id+ "/"+QString::number(Tools::get_1970_ms());
        only_update_property_to_model(_current_editing_picture_id, new_property);
        if(DEBUG)
            Log::d(TAG,"反撤销参数还原成功");
        QString state_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_State_Integer);
        ImageState old_state = ImageState(temp._current_property[state_key].toInt());
        if(_current_visible_property_index == _picture_old_visible_property.size() -1)
            setCan_redo(false);
        setCan_undo(true);
        if(old_state != _current_image_state)
        {
            setCurrent_editing_picture_state(int(old_state)); //切状态会同步ui，不需要返回参数去同步ui
            return QJsonObject();
        }
        else
            return  temp._current_property;
    }
    else
    {
        Log::e(TAG,"反撤销参数还原失败");
        setCan_redo(false);
        return QJsonObject();
    }
}

void PictureManager::save_current_state()
{
    Picture_Visible_Propertys current;
    bool ok = get_visible_value(_current_editing_picture_id,current._current_property);
    if(ok)
    {
        while(_picture_old_visible_property.size() -1 > _current_visible_property_index)
            _picture_old_visible_property.pop_back();
        current._picture_id = _current_editing_picture_id;
        current.stack = _picture_old_visible_property.size();
        _picture_old_visible_property.push_back(current);
        _current_visible_property_index = _picture_old_visible_property.size() -1;
        qDebug() << "当前状态索引" << _current_visible_property_index << "总历史轨迹" << _picture_old_visible_property.size();
        if(_picture_old_visible_property.size() > 1)
            setCan_undo(true);
        else
            setCan_undo(false);
        setCan_redo(false);
    }
    else
    {
        qDebug() << "图片id，无法获取参数,保存状态失败" <<_current_editing_picture_id;
    }
}

void PictureManager::clear_all_states()
{
    _picture_old_visible_property.clear();
    setCan_redo(false);
    setCan_undo(false);
    _current_visible_property_index = -1;
}

void PictureManager::set_editing_picture_id(const QString &picture_id)//,int state)
{
    setCurrent_editing_picture_id(picture_id);
    // setCurrent_editing_picture_state(state);
    //_current_image_state= ImageState(state);
}

void PictureManager::set_collecting_adult_teeth_indexs(const QJsonArray &indexs)
{
    _collecting_adult_teeth_indexs = indexs;
}

void PictureManager::set_collecting_kit_teeth_indexs(const QJsonArray &indexs)
{
    _collecting_kid_teeth_indexs = indexs;
}

void PictureManager::set_collecting_adult_teeth_text(const QJsonArray &text)
{
    _collecting_adult_teeth_text = text;
}

void PictureManager::set_collecting_kit_teeth_text(const QJsonArray &text)
{
    _collecting_kid_teeth_text = text;
}

void PictureManager::set_collected_adult_teeth_indexs(const QJsonArray &indexs)
{
    if(indexs != _fillter_for_patient_page_collected_picture->_collected_adult_teeth_indexs)
    {
        _fillter_for_patient_page_collected_picture->_collected_adult_teeth_indexs = indexs;
        emit collected_choosed_tee_indexsChanged();
    }
}

void PictureManager::set_collected_kit_teeth_indexs(const QJsonArray &indexs)
{
    if(_fillter_for_patient_page_collected_picture->_collected_kid_teeth_indexs != indexs)
    {
        _fillter_for_patient_page_collected_picture->_collected_kid_teeth_indexs = indexs;
        emit collected_choosed_tee_indexsChanged();
    }
}

void PictureManager::set_collected_adult_teeth_text(const QJsonArray &text)
{
    if(text != _fillter_for_patient_page_collected_picture->_collected_adult_teeth_text)
    {
        _fillter_for_patient_page_collected_picture->_collected_adult_teeth_text = text;
        emit collected_choosed_tee_textChanged();
    }
}

void PictureManager::set_collected_kit_teeth_text(const QJsonArray &text)
{
    if(_fillter_for_patient_page_collected_picture->_collected_kid_teeth_text != text)
    {
        _fillter_for_patient_page_collected_picture->_collected_kid_teeth_text = text;
        emit collected_choosed_tee_textChanged();
    }
}

void PictureManager::save_image(const QImage &pic, const QJsonObject &info)
{


    QImage temp1 = pic.convertToFormat(QImage::Format::Format_RGB888);
    int rotate = info["rotate"].toInt();
    cv::Mat  src2 = cv::Mat(temp1.height(), temp1.width(), CV_8UC3, (void*)temp1.constBits(), temp1.bytesPerLine());
    cv::Mat ret ;
    if(rotate == -90)
        rotate = 270;
    else if(rotate == -180)
        rotate = 180;
    else if (rotate == -270)
        rotate = 90;
    else if(rotate == -360)
        rotate = 0;

    if(rotate == 90)
    {
        cv::Mat temp;
        transpose(src2,ret);
        flip(ret,temp,1);
        ret = temp;
    }
    else if(rotate == 180)
    {
        cv::flip(src2,ret,-1);
    }
    else if(rotate == 270)
    {
        cv::Mat temp;
        transpose(src2,ret) ;
        flip(ret,temp,0);
        ret = temp;
    }
    else
    {

    }
    if(ret.empty() == false)
    {
        temp1 = QImage(ret.data, ret.cols,ret.rows,3*sizeof (uchar) *ret.cols,QImage::Format::Format_RGB888).copy();
    }

    if(info["task"].toString() == "get_final_image")
        got_report_image(temp1, info);
    else
    {
        QString color = info["color"].toString();
        QString path = info["path"].toString();
        qDebug() << path;


        if(path.endsWith(".dcm"))
        {
            temp1 = temp1.convertToFormat(QImage::Format::Format_RGB888);
            imageSaveDCMIMG(temp1,path);
        }
        else{
            if(color == "rgba")
            {
                bool ok =   temp1.save(path);
                if(DEBUG)
                {
                    Log::d(TAG,QString("文件导出")+(ok ?  "成功" : "失败"));
                }
            }
            else if(color == "16gray")
            {
                QImage temp = temp1.convertToFormat(QImage::Format::Format_Grayscale16);
                bool ok = temp.save(path);
                if(DEBUG)
                {
                    Log::d(TAG,QString("文件导出")+(ok ?  "成功" : "失败"));
                }
            }
            else
            {
                qDebug() << "导出文件参数错误" << info;
            }
        }
    }

}

void PictureManager::save_default_visible_params(const QJsonObject &params)
{
    QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat);
    settings.setValue("default_visible_params",params);
    Configure::set_default_visible_propertys(params);
}

QString PictureManager::reportImageCopy()
{
    QString path = QApplication::applicationDirPath() + "/Images/1.png";
    if (path.isEmpty() )//路径是否存在
        QFile::remove(path);
    return "file:///"+path;
}

void PictureManager::reverseColorPicture(const QString &picture_id, const QString &key)
{
    QJsonObject new_property;
    new_property[key] = true;
    QString picture_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT);
    for(int i = 0;i < _all_data.size();i++)
    {
        QJsonObject & ele = _all_data[i];
        if(ele[picture_key].toString() == picture_id)
        {
            new_property[key] = ele[key] == true? false : true;
            break;
        }
    }
    _change_state_property = QJsonObject();
    _update_picture_property_any_where(picture_id,new_property,false,true);
}

bool PictureManager::get_final_image(const QString &id)//, QImage &out)
{
    QString temp = "no_path";
    QString origin_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_State_Integer);
    QString fake_color_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Enable_Fake_Color_Filter_BOOLEAN);
    QString rotate_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Rotate_integer);
    QJsonArray temp2;

    QJsonObject params = get_params(id);
    ImageState state = ImageState(params[origin_key].toInt());
    int rotate = params[rotate_key].toInt();
    QString result_color;
    if(state == ImageState::Origin)
    {
        result_color = "16gray";
    }
    else {
        bool fake_color = params[fake_color_key].toBool();
        if(fake_color == false)
            result_color = "16gray";
        else
            result_color = "rgba";
    }
    QJsonObject info;
    info["rotate"] = rotate;
    info["picture_id"] = id;
    info["path"] = temp;
    info["color"] = result_color;
    info["task"] = "get_final_image";
    temp2.append(info);
    begin_export_pictures(temp2);
    return true;

    //    QJsonObject picture;// = _all_data.find(id);
    //    ImagesTable * tb = ImagesTable::getInstance();
    //    QString picture_key = tb->get_field_name(ImagesTable::Field::Image_ID_TEXT);
    //    for(int i =0; i < _all_data.size(); i++)
    //    {
    //        const QJsonObject & ele  = _all_data.at(i);
    //        if(ele[picture_key].toString() == id)
    //        {
    //            picture = ele;
    //            break;
    //        }
    //    }
    //    if(picture.isEmpty())
    //        return false;
    //    auto temp = ImageFactory::get_instance()->get_final_image(id,out,picture);
    //    return  temp;
}


QJsonObject PictureManager::on_new_image(const QString &path, const QString &last_state_path, const QString & pillar_path, int width, int height, qint64 pts, ImagesTable::PictureSoureType source_type, int w, int h,float hor_pix_space_mm,float ver_pix_space_mm)
{
    auto table = ImagesTable::getInstance();
    QJsonObject new_picture;
    QJsonArray collecting_indexs; //收集中牙齿编号
    QJsonArray collected_indexs;
    QJsonArray collecting_text;//收集中牙齿型号
    QJsonArray collected_text;
    qDebug() << "--------------" << _collecting_tee_type << _collecting_kid_teeth_indexs;
    if(_collecting_tee_type == "kid"){
        collecting_indexs = _collecting_kid_teeth_indexs;
        collecting_text = _collecting_kid_teeth_text;
    }
    else{
        collecting_indexs = _collecting_adult_teeth_indexs;
        collecting_text = _collecting_adult_teeth_text;
    }
    //    _tee_indexs_selecter_provider_for_collecting_image->get_database_indexs_format(collecting_indexs);


    if(_fillter_for_patient_page_collected_picture->_collected_tee_type == "kid"){
        collected_indexs = _fillter_for_patient_page_collected_picture->_collected_kid_teeth_indexs;
        collected_text = _fillter_for_patient_page_collected_picture->_collected_kid_teeth_text;
    }
    else{
        collected_indexs = _fillter_for_patient_page_collected_picture->_collected_adult_teeth_indexs;
        collected_text = _fillter_for_patient_page_collected_picture->_collected_adult_teeth_text;
    }
    //  _fillter_for_patient_page_collected_picture->_collected_tee_indexs_selecter_image_provider->get_database_indexs_format(collected_indexs);
    bool ok = table->create_and_init_a_picture_and_save_to_database(new_picture,path,last_state_path,pillar_path,width,height,pts,_patient_id,_collecting_tee_type,/*_collecting_tee_from,*/collecting_indexs,collecting_text,source_type,w,h,hor_pix_space_mm,ver_pix_space_mm,_voltage_v,_electricity_a,_expose_second_index,_expose_cm, _expose_second_value);
    if(!ok)
    {
        qDebug() << "创建图片且保存到数据库失败";
        return new_picture;
    }
    _all_data.append(new_picture);
    // _pictures_old_visible_property[QString::number(pts)] = Picture_Visible_Property_Trail();

    { //先把图片数据放到图片集合里，后面有需要再加载
        ImageFactory::ImageData data;
        data.last = std::shared_ptr<QImage>(0);
        data.origin = std::shared_ptr<QImage>(0);
        QString origin_path_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_Path_TEXT);
        QString last_state_path_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_Last_State_Path_TEXT);
        QString image_state_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_State_Integer);
        QString rui_hua_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Rui_Hua_integer);

        data.origin_path = new_picture[origin_path_key].toString();
        data.last_path = new_picture[last_state_path_key].toString();
        data.state = ImageState(new_picture[image_state_key].toInt());
        data.Rui_Hua_integer = new_picture[rui_hua_key].toInt();
        ImageFactory::get_instance()->add_image(QString::number(pts),data);
        //_editing_iamge_provider->addImage(QString::number(pts),data);

    }
    qDebug() <<"追加新图片" << new_picture;
    _today_picture_model->append(new_picture);
    if(_today_picture_model->getCount() > 4)
        _today_picture_model->remove_first();


    ok = test_filter(new_picture,
                     _fillter_for_patient_page_collected_picture->collected_source_type,
                     _fillter_for_patient_page_collected_picture->collected_level,
                     _fillter_for_patient_page_collected_picture->collected_has_tee_type,
                     _fillter_for_patient_page_collected_picture->collected_date_begin,
                     _fillter_for_patient_page_collected_picture->collected_date_end,
                     collected_indexs,
                     collected_text,
                     _fillter_for_patient_page_collected_picture->_collected_tee_type);

    if(ok)
    {
        _collected_picture_model->append(new_picture);
    }
    return  new_picture;
}

PictureModel *PictureManager::getPictureModel()
{
    return _collected_picture_model;
}

//QObject *PictureManager::getToday_picture_model()
//{
//    return _today_picture_model;
//}

EditingPictureModel *PictureManager::get_editing_model()
{
    return  _editing_picture_model;
}

QString PictureManager::getCollecting_lastest_picture_url() const
{
    if(_collecting_lastest_picture_url == "")
        return  "";
    else
        return  Tools::get_root_path() +"/" + _collecting_lastest_picture_url;
}

void PictureManager::setCollecting_lastest_picture_url(const QString &collecting_lastest_picture_url)
{
    if(_collecting_lastest_picture_url != collecting_lastest_picture_url)
    {
        _collecting_lastest_picture_url = collecting_lastest_picture_url;
        emit collecting_lastest_picture_urlChanged();
    }
}



void PictureManager::setLastest_picture_id(const QString &lastest_picture_id)
{
    _lastest_picture_id = lastest_picture_id;
}

void PictureManager::on_delete_this_paitent(qint64 patient_id)
{ //<<
    auto imagetable_instance1 = ImagesTable::getInstance();
    imagetable_instance1->remove_this_patient_all_picture(patient_id);
    if(patient_id == _patient_id)
    {

        QString key1 = imagetable_instance1->get_field_name(ImagesTable::Field::Image_Path_TEXT);
        QString key2 = imagetable_instance1->get_field_name(ImagesTable::Field::Image_Last_State_Path_TEXT);
        QString key3 = imagetable_instance1->get_field_name(ImagesTable::Field::Image_Pillar_Path_TEXT);
        QStringList * files = new QStringList;
        for(int i =0; i < _all_data.size(); i ++ )
        {
            const QJsonObject info = _all_data.at(i);
            *files << info[key1].toString();
            *files << info[key2].toString();
            *files << info[key3].toString();
        }
        _all_data.clear();
        _today_picture_model->clear();
        _collected_picture_model->clear();
        _editing_picture_model->reset_to_empty(m_edit_grid_index);
        _patient_id = -1;
        //  setCollecting_lastest_picture_url("");
        set_last_pic_info(QJsonObject());
        setLastest_picture_id("-1");

        auto line_manager = LineManager::get_instance();
        line_manager->remove_this_patient_all_lines(patient_id);
        if(files->size() > 0)
        {
            std::thread t([files,this]{
                for(auto it = files->begin(); it != files->end();it++)
                {
                    QFile f(*it);
                    if(f.exists())
                    {
                        bool ok =   f.remove();
                        if(!ok)
                            Log::e(TAG,"删除文件失败:"+(*it));
                    }
                    else
                        Log::i(TAG,"删除文件，但是文件不存在:"+(*it));
                }
                delete files;
            });
            t.detach();
        }
        else
            delete files;
    }

}

void PictureManager::clear_fixed_image()
{
    get_fixed_image_provider()->set_dark_image_data(QByteArray());
    get_fixed_image_provider()->set_light_image_data(QByteArray());
}

void PictureManager::get_usb_info()
{
    _usb_reader->get_usb_info();
}

QString PictureManager::get_dark_pic_dir()
{
    QString root_dir = Configure::data_store_root_dir;
    QString file_name = root_dir  + "/fixed/"+ get_usb_ser_num() + "_dark.raw";
    return file_name;
}

QString PictureManager::get_light_pic_dir()
{
    QString root_dir = Configure::data_store_root_dir;
    QString file_name = root_dir  + "/fixed/"+ get_usb_ser_num() + "_light.raw";
    return  file_name;
}

QString PictureManager::get_factory_dark_pic_dir()
{
    QString root_dir = Configure::data_store_root_dir;
    return  root_dir + "/factory_fixed/"+ get_usb_ser_num() + "_dark.raw";
}

QString PictureManager::get_factory_light_pic_dir()
{
    QString root_dir = Configure::data_store_root_dir;
    return   root_dir  + "/factory_fixed/"+ get_usb_ser_num() + "_light.raw";
}

QString PictureManager::get_usb_dark_pic_dir(const QString & root_dir)
{

    QString file_name = root_dir  + "/"+ get_usb_ser_num() + "_dark.raw";
    return file_name;
}

QString PictureManager::get_usb_light_pic_dir(const QString & root_dir)
{
    QString file_name = root_dir  + "/"+ get_usb_ser_num() + "_light.raw";
    return  file_name;
}

bool PictureManager::save_dark_image(const QByteArray &data)
{

    QFile   file(get_dark_pic_dir());
    bool ok =  file.open(QIODevice::WriteOnly);
    if(ok)
        file.write(data);
    return ok;

}

bool PictureManager::save_light_image(const QByteArray &data)
{

    QFile   file(get_light_pic_dir());
    bool ok =  file.open(QIODevice::WriteOnly);
    if(ok)
        file.write(data);
    return ok;
}

QByteArray PictureManager::load_dark_image()
{

    QString file_name = get_dark_pic_dir();
    QFile   file(file_name);
    if(!file.exists())
        return QByteArray();
    bool ok =  file.open(QIODevice::ReadOnly);
    if(ok)
        return file.readAll();
    else
        return QByteArray();
}

QByteArray PictureManager::load_light_image()
{

    QString file_name = get_light_pic_dir();
    QFile   file(file_name);
    if(!file.exists())
        return QByteArray();
    bool ok =  file.open(QIODevice::ReadOnly);
    if(ok)
        return file.readAll();
    else
        return QByteArray();
}

void PictureManager::reset_factor_image()
{
    auto st = Settings::get_instance();
    // QString root_dir = Configure::data_store_root_dir;
    QString file_name_dark_des = get_dark_pic_dir();
    QString file_name_light_des = get_light_pic_dir();

    QString file_name_dark_source = get_factory_dark_pic_dir(); // root_dir  + "/factory_fixed/"+ get_usb_ser_num() + "_dark.raw";
    QString file_name_light_source =get_factory_light_pic_dir();// root_dir  + "/factory_fixed/"+ get_usb_ser_num() + "_light.raw";
    {
        QFile fd_s(file_name_dark_source);
        QFile fl_s(file_name_light_source);
        if(!fd_s.exists() || !fl_s.exists())
        {
            show_message("",st->tran("出厂文件丢失，请联系传感器设备厂家"));
            return;
        }
    }
    {
        QFile fd_d(file_name_dark_des);
        QFile fl_d(file_name_light_des);
        if(fd_d.exists())
            fd_d.remove();
        if(fl_d.exists())
            fl_d.remove();
    }
    bool ok1=  QFile::copy(file_name_dark_source,file_name_dark_des);
    bool ok2 = QFile::copy(file_name_light_source,file_name_light_des);

    if( ok1 && ok2)
    {
        show_message("",st->tran("成功"));
    }
    else
    {
        show_message("",st->tran("失败"));
    }
}


void PictureManager::reset(qint64 new_patient_id)
{
    _today_picture_model->clear();
    _editing_picture_model->reset_to_empty(m_edit_grid_index);
    setEediting_count(_editing_picture_model->getCount());
    _collected_picture_model->clear();
    _current_visible_property_index = -1;
    _picture_old_visible_property.clear();
    _all_data.clear();
    setCurrent_editing_picture_id("-1");
    ImageFactory::get_instance()->clear_image();
    // _editing_iamge_provider->clearImage();

    _patient_id = new_patient_id;
    // setCollecting_lastest_picture_url("");//最新的采集图片url
    set_last_pic_info(QJsonObject());
    _lastest_picture_id = "-1"; //最后的图片id
    reset_collected_page_picture_filter(); //复位条件
    //    _tee_indexs_selecter_collecting_image_provider->init(); //已经注释掉,采集的牙齿页面状态不变
    //    setCollecting_tee_type("adult");

}

bool PictureManager::test_filter(const QJsonObject &picture, ImagesTable::PictureSoureType picture_soure_type, ImagesTable::PictureLevel picture_level, ImagesTable::PictureHasTeeType picture_has_tee_type, qint64 begin, qint64 end,const QJsonArray &tee_indexs, const QJsonArray &tee_text, const QString &tee_type)
{
    bool picture_type_ok = picture_soure_type == ImagesTable::PictureSoureType::All ||  int(picture_soure_type) == picture[ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Source_Type_integer)].toInt();
    bool picture_level_ok = picture_level == ImagesTable::PictureLevel::All || int(picture_level) == picture[ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Level_integer)].toInt();
    bool picture_has_tee_type_ok = false;
    if(picture_has_tee_type == ImagesTable::PictureHasTeeType::All)
        picture_has_tee_type_ok = true;
    else
    {
        QString picture_tee_indexs_str = picture[ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Tee_Indexs_TEXT)].toString();
        QJsonArray picture_tee_indexs;
        Tools::str_2_jsonarray(picture_tee_indexs_str,picture_tee_indexs);
        QString picture_tee_text_str = picture[ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Tee_TEXT)].toString();
        QJsonArray picture_tee_text;
        Tools::str_2_jsonarray(picture_tee_text_str,picture_tee_text);
        if(picture_has_tee_type == ImagesTable::PictureHasTeeType::NoTeeType)
        {
            picture_has_tee_type_ok = picture_tee_indexs.size() == 0;
        }
        else
        {
            if(tee_type == picture[ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Tee_Type_TEXT)].toString())
            {
                for (const auto & v1 : picture_tee_indexs)
                {
                    for (const auto & v2 : tee_indexs)
                    {
                        if(v1.toInt() == v2.toInt())
                        {
                            picture_has_tee_type_ok  = true;
                            break;
                        }
                    }
                    if(picture_has_tee_type_ok  == true)
                        break;
                }
            }else
                picture_has_tee_type_ok = false;
        }
    }

    qint64 pts = picture[ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT)].toString().toLongLong();
    bool date_ok = pts > begin && pts <end;
    qDebug() << picture_type_ok << picture_level_ok << picture_has_tee_type_ok << date_ok;
    return  picture_type_ok && picture_level_ok && picture_has_tee_type_ok && date_ok;
}

bool PictureManager::get_visible_value(const QString &picture_id, QJsonObject &out)
{
    QString picture_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT);
    ImagesTable::Field need_fields[14] = {ImagesTable::Field::Window_Begin_integer,
                                          ImagesTable::Field::Window_End_integer,
                                          //   ImagesTable::Field::Scale_float,
                                          ImagesTable::Field::Rotate_integer,
                                          ImagesTable::Field::Luminance_float,
                                          ImagesTable::Field::Contrast_float,
                                          ImagesTable::Field::Gama_float,
                                          ImagesTable::Field::Float_float,
                                          ImagesTable::Field::Rui_Hua_integer,
                                          ImagesTable::Field::Enable_Fake_Color_Filter_BOOLEAN,
                                          ImagesTable::Field::Enable_Turn_Color_Filter_BOOLEAN,
                                          ImagesTable::Field::Filter_Index_integer,
                                          ImagesTable::Field::Enable_Hor_Mirror_BOOLEAN,
                                          ImagesTable::Field::Enable_Ver_Mirror_BOOLEAN,
                                          ImagesTable::Field::Image_State_Integer
                                         };

    for(int i =0;i <_all_data.size();i++)
    {
        const QJsonObject & ele = _all_data.at(i);
        if(ele[picture_key] == picture_id)
        {

            for(int j = 0; j < 14;j++)
            {
                QString key = ImagesTable::getInstance()->get_field_name(need_fields[j]);
                out[key] = ele[key];
            }
            return true;
        }
    }
    Log::e(TAG,"代码逻辑错误,保存当前可见属性时，无法找到id:"+picture_id);
    return false;
}

void PictureManager::clear_usb_reader(UsbReader *temp)
{
    QThread::msleep(150);
    QTimer::singleShot(1000,[temp]{
        temp->deleteLater();
    });
}

bool PictureManager::getShow_mark_bar() const
{
    return _show_mark_bar;
}

void PictureManager::setShow_mark_bar(bool show_mark_bar)
{
    if(show_mark_bar != _show_mark_bar)
    {

        _show_mark_bar = show_mark_bar;
        QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat);
        settings.setValue("_show_mark_bar",_show_mark_bar);
        emit show_mark_barChanged(); //标注栏
    }
}

bool PictureManager::getShow_mark_manager_bar() const
{
    return _show_mark_manager_bar;
}

void PictureManager::setShow_mark_manager_bar(bool show_mark_manager_bar)
{
    if(show_mark_manager_bar != _show_mark_manager_bar)
    {

        _show_mark_manager_bar = show_mark_manager_bar;
        QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat);
        settings.setValue("_show_mark_manager_bar",_show_mark_manager_bar);
        emit show_mark_manager_barChanged(); //标注管理栏
    }
}

bool PictureManager::getShow_rui_hua_bar() const
{
    return _show_rui_hua_bar;
}

void PictureManager::setShow_rui_hua_bar(bool show_rui_hua_bar)
{
    if(show_rui_hua_bar != _show_rui_hua_bar)
    {
        _show_rui_hua_bar = show_rui_hua_bar;
        QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat);
        settings.setValue("_show_rui_hua_bar",_show_rui_hua_bar);
        emit show_rui_hua_barChanged(); //锐化栏
    }
}

bool PictureManager::getShow_meter_bar() const
{
    return _show_meter_bar;
}

void PictureManager::setShow_meter_bar(bool show_meter_bar)
{
    if(show_meter_bar != _show_meter_bar)
    {
        _show_meter_bar = show_meter_bar;
        QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat);
        settings.setValue("_show_meter_bar",_show_meter_bar);
        emit show_meter_barChanged(); //测量栏
    }
}

bool PictureManager::getShow_color_space_bar() const
{
    return _show_color_space_bar;
}

void PictureManager::setShow_color_space_bar(bool show_color_space_bar)
{
    if(show_color_space_bar != _show_color_space_bar)
    {
        _show_color_space_bar = show_color_space_bar;
        QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat);
        settings.setValue("_show_color_space_bar",_show_color_space_bar);
        emit show_color_space_barChanged(); //色域调节栏
    }
}

bool PictureManager::getShow_filter_bar() const
{
    return _show_filter_bar;
}

void PictureManager::setShow_filter_bar(bool show_filter_bar)
{
    if(show_filter_bar != _show_filter_bar)
    {
        _show_filter_bar = show_filter_bar;
        QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat);
        settings.setValue("_show_filter_bar",_show_filter_bar);
        emit show_filter_barChanged(); //滤镜栏
    }
}

bool PictureManager::getShow_fix_image_bar() const
{
    return _show_fix_image_bar;
}

void PictureManager::setShow_fix_image_bar(bool show_fix_image_bar)
{
    if(show_fix_image_bar != _show_fix_image_bar)
    {
        _show_fix_image_bar = show_fix_image_bar;
        QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat);
        settings.setValue("_show_fix_image_bar",_show_fix_image_bar);
        emit show_fix_image_barChanged(); //影像修复栏
    }
}

bool PictureManager::getShow_transformation_bar() const
{
    return _show_transformation_bar;
}

void PictureManager::setShow_transformation_bar(bool show_transformation_bar)
{
    if(_show_transformation_bar != show_transformation_bar)
    {
        _show_transformation_bar = show_transformation_bar;
        QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat);
        settings.setValue("_show_transformation_bar",_show_transformation_bar);
        emit show_transformation_barChanged(); //变换栏
    }
}

int PictureManager::getSource_index() const
{
    return _source_index;
}

void PictureManager::setSource_index(int source_index)
{
    if(_source_index != source_index)
    {
        _source_index = source_index;
        QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat);
        settings.setValue("_source_index",source_index);
        emit source_indexChanged();
    }
}

QString PictureManager::getExpose_cm() const
{
    return _expose_cm;
}

void PictureManager::setExpose_cm(const QString &expose_cm)
{
    if(_expose_cm != expose_cm)
    {
        _expose_cm = expose_cm;
        QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat);
        settings.setValue("_expose_cm",_expose_cm);
        emit expose_cmChanged();
    }
}

QString PictureManager::getElectricity_a() const
{
    return _electricity_a;
}

void PictureManager::setElectricity_a(const QString &electricity_a)
{
    if(_electricity_a != electricity_a)
    {
        _electricity_a = electricity_a;
        QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat);
        settings.setValue("_electricity_a",_electricity_a);
        emit electricity_aChanged();
    }
}

int PictureManager::getExpose_second_index() const
{
    return _expose_second_index;
}

void PictureManager::setExpose_second_index(int expose_second_index)
{
    if(expose_second_index != _expose_second_index)
    {
        _expose_second_index = expose_second_index;
        QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat);
        settings.setValue("_expose_second_index",_expose_second_index);
        emit expose_second_indexChanged();
    }
}

QString PictureManager::getExpose_second_value() const
{
    return  _expose_second_value;
}

void PictureManager::setExpose_second_value(QString expose_second_value)
{
    if(expose_second_value != _expose_second_value)
    {
        _expose_second_value = expose_second_value;
        QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat);
        settings.setValue("_expose_second_value", _expose_second_value);
        emit expose_second_valueChanged();
    }
}

QString PictureManager::getVoltage_v() const
{
    return _voltage_v;
}

void PictureManager::setVoltage_v(const QString &voltage_v)
{
    if(_voltage_v != voltage_v )
    {
        _voltage_v  = voltage_v ;
        QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat);
        settings.setValue("_voltage_v",_voltage_v );
        emit voltage_vChanged();
    }
}


bool PictureManager::getCan_redo() const
{
    return _can_redo;
}

void PictureManager::setCan_redo(bool can_redo)
{
    if(_can_redo != can_redo)
    {
        _can_redo = can_redo;
        emit can_redoChanged();
    }
}

bool PictureManager::getCan_undo() const
{
    return _can_undo;
}

void PictureManager::setCan_undo(bool can_undo)
{
    if(_can_undo != can_undo)
    {
        _can_undo = can_undo;
        emit can_undoChanged();
    }
}

int PictureManager::getCurrent_editing_picture_state() const
{
    return int(_current_image_state);
}

void PictureManager::setCurrent_editing_picture_state(int current_editing_picture_state)
{

    if(int(_current_image_state) != current_editing_picture_state)
    {
        _current_image_state = ImageState(current_editing_picture_state);
        emit current_editing_picture_stateChanged();
    }
}

QString PictureManager::getCurrent_editing_picture_id() const
{
    return _current_editing_picture_id;
}

void PictureManager::setCurrent_editing_picture_id(const QString &current_editing_picture_id)
{
    if(_current_editing_picture_id != current_editing_picture_id)
    {
        _current_editing_picture_id = current_editing_picture_id;
        emit current_editing_picture_idChanged();
    }
}

int PictureManager::getSreen_height() const
{
    return _sreen_height;
}

void PictureManager::setSreen_height(int sreen_height)
{
    _sreen_height = sreen_height;
}

int PictureManager::getSreen_width() const
{
    return _sreen_width;
}

void PictureManager::setSreen_width(int sreen_width)
{
    _sreen_width = sreen_width;
}




QString PictureManager::getCollecting_tee_type() const
{
    return  _collecting_tee_type;
}

void PictureManager::setCollecting_tee_type(const QString &collecting_tee_type)
{
    if(_collecting_tee_type != collecting_tee_type)
    {
        _collecting_tee_type =collecting_tee_type;
        emit collecting_tee_typeChanged();
    }
}

//void PictureManager::setCollecting_tee_from(const QString &collecting_tee_from)
//{
//    if(_collecting_tee_from != collecting_tee_from)
//    {
//        _collecting_tee_from =collecting_tee_from;
//    }
//}

int PictureManager::getEditing_count() const
{
    return _editing_count;
}

void PictureManager::setEediting_count(int value)
{
    if(_editing_count != value)
    {
        _editing_count = value;
        emit editing_countChanged();
    }
}


//TeeIndexsSelecterImageProvider *PictureManager::get_collecting_tee_indexs_select_iamge_for_collecting_provider()
//{
//    return _tee_indexs_selecter_provider_for_collecting_image;
//}

//TeeIndexsSelecterImageProvider *PictureManager::get_collected_tee_indexs_select_iamge_for_collecting_provider()
//{
//    return _fillter_for_patient_page_collected_picture->_collected_tee_indexs_selecter_image_provider;
//}

//TeeIndexsSelecterImageProvider *PictureManager::get_collected_tee_indexs_select_iamge_for_picture_info_provider()
//{
//    return _tee_indexs_selecter_provider_for_picture_info;
//}

EditingImageProvider *PictureManager::get_editing_iamge_provider()
{
    return _editing_iamge_provider;
}

QString PictureManager::getVer_shader()
{
    return  _ver_shader;
}

QString PictureManager::getFram_shader()
{
    return _fram_shader;
}


QString PictureManager::getCollected_tee_type() const
{
    return  _fillter_for_patient_page_collected_picture->_collected_tee_type;
}

void PictureManager::setCollected_tee_type(const QString &collected_tee_type)
{
    if(_fillter_for_patient_page_collected_picture->_collected_tee_type != collected_tee_type)
    {
        _fillter_for_patient_page_collected_picture->_collected_tee_type = collected_tee_type;
        //_fillter_for_patient_page_collected_picture->_collected_tee_indexs_selecter_image_provider->select(collected_tee_type);
        //emit collected_picture_url_change(_fillter_for_patient_page_collected_picture->_collected_tee_indexs_selecter_image_provider->get_url()); //更新图片
        emit collected_tee_typeChanged();
    }
}


int PictureManager::getCollected_source_type() const
{
    return int(_fillter_for_patient_page_collected_picture->collected_source_type);
}

void PictureManager::setCollected_source_type(int value)
{
    if(value != int(_fillter_for_patient_page_collected_picture->collected_source_type))
    {
        _fillter_for_patient_page_collected_picture->collected_source_type = ImagesTable::PictureSoureType(value);
        emit collected_source_typeChanged();
    }
}

int PictureManager::getCollected_level() const
{
    return int(_fillter_for_patient_page_collected_picture->collected_level);
}

void PictureManager::setCollected_level(int value)
{
    if(value != int(_fillter_for_patient_page_collected_picture->collected_level))
    {
        _fillter_for_patient_page_collected_picture->collected_level = ImagesTable::PictureLevel(value);
        emit collected_levelChanged();
    }
}

int PictureManager::getCollected_has_tee_type() const
{
    return  int(_fillter_for_patient_page_collected_picture->collected_has_tee_type);
}

void PictureManager::setCollected_has_tee_type(int value)
{
    if(value != int(_fillter_for_patient_page_collected_picture->collected_has_tee_type))
    {
        _fillter_for_patient_page_collected_picture->collected_has_tee_type = ImagesTable::PictureHasTeeType(value);
        emit collected_has_tee_typeChanged();
    }
}

qint64 PictureManager::getCollected_date_begin() const
{
    return qint64(_fillter_for_patient_page_collected_picture->collected_date_begin);
}

void PictureManager::setCollected_date_begin(const qint64 &value)
{
    if(value != _fillter_for_patient_page_collected_picture->collected_date_begin)
    {
        _fillter_for_patient_page_collected_picture->collected_date_begin = value;
        emit collected_date_beginChanged();
    }
}

qint64 PictureManager::getCollected_date_end() const
{
    return _fillter_for_patient_page_collected_picture->collected_date_end;
}

void PictureManager::setCollected_date_end(const qint64 &value)
{

    if(value != _fillter_for_patient_page_collected_picture->collected_date_end)
    {
        _fillter_for_patient_page_collected_picture->collected_date_end = value;
        emit collected_date_endChanged();
    }
}


void PictureManager::update_selected_picture()
{
    qDebug() << "_fillter_for_patient_page_collected_picture->collected_has_tee_type: " << int(_fillter_for_patient_page_collected_picture->collected_has_tee_type);
    if(_fillter_for_patient_page_collected_picture->drop_signal == true)
        return;
    QString picture_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT);
    QString checked_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Checked_For_Add_To_Paint_List_Bool);
    QJsonArray collected_indexs;
    QJsonArray collected_text;
    if(_fillter_for_patient_page_collected_picture->_collected_tee_type == "kid"){
        collected_indexs = _fillter_for_patient_page_collected_picture->_collected_kid_teeth_indexs;
        collected_text = _fillter_for_patient_page_collected_picture->_collected_kid_teeth_text;
    }
    else{
        collected_indexs = _fillter_for_patient_page_collected_picture->_collected_adult_teeth_indexs;
        collected_text = _fillter_for_patient_page_collected_picture->_collected_adult_teeth_text;
    }
    //_fillter_for_patient_page_collected_picture->_collected_tee_indexs_selecter_image_provider->get_database_indexs_format(collected_indexs);

    for(int i =0;i < _all_data.size();i++)
    {
        const QJsonObject & ele = _all_data.at(i);
        bool ok = test_filter(ele, _fillter_for_patient_page_collected_picture->collected_source_type,
                              _fillter_for_patient_page_collected_picture->collected_level,
                              _fillter_for_patient_page_collected_picture->collected_has_tee_type,
                              _fillter_for_patient_page_collected_picture->collected_date_begin,
                              _fillter_for_patient_page_collected_picture->collected_date_end,
                              collected_indexs,
                              collected_text,
                              _fillter_for_patient_page_collected_picture->_collected_tee_type);
        if(ok)
        {
            qDebug() << "_collected_picture_model->append";
            _collected_picture_model->append(ele);
            if(ele[checked_key].toBool())
            {

                _editing_picture_model->append(ele);
                _collected_picture_model->checked_thoes_ids(_editing_picture_model->get_checked_ids());
            }
        }
        else
        {
            QString picture_id = ele[picture_key].toString();
            qDebug() << "_collected_picture_model->remove";
            _collected_picture_model->remove(picture_id);

            _editing_picture_model->remove(picture_id);
        }

    }
    setEediting_count(_editing_picture_model->getCount());
}




PictureManager::FillterForPatientPageCollectedPicture::FillterForPatientPageCollectedPicture()
{
    //_collected_tee_indexs_selecter_image_provider = new TeeIndexsSelecterImageProvider;
    // _collected_tee_indexs_selecter_image_provider->init();
    QDateTime now = QDateTime::currentDateTime();
    auto b= now.addDays(-365).toMSecsSinceEpoch(); //2周前

    now.setTime(QTime(23,59));

    auto e= now.toMSecsSinceEpoch(); //2周前


    collected_source_type = ImagesTable::PictureSoureType::All;

    collected_has_tee_type = ImagesTable::PictureHasTeeType::All;
    collected_date_begin=b;
    collected_date_end=e;


    collected_level =ImagesTable::PictureLevel::All;

    _collected_adult_teeth_indexs = QJsonArray();
    _collected_kid_teeth_indexs = QJsonArray();
    reinit();
}


void PictureManager::FillterForPatientPageCollectedPicture::reinit()
{


    //    Q_PROPERTY(int      collected_source_type        READ getCollected_source_type   WRITE setCollected_source_type   NOTIFY collected_source_typeChanged);
    //    Q_PROPERTY(int      collected_level        READ getCollected_level  WRITE setCollected_level     NOTIFY collected_levelChanged);
    //    Q_PROPERTY(int      collected_has_tee_type        READ getCollected_has_tee_type  WRITE setCollected_has_tee_type     NOTIFY collected_has_tee_typeChanged);
    //    Q_PROPERTY(qint64      collected_date_begin        READ getCollected_date_begin  WRITE setCollected_date_begin     NOTIFY collected_date_beginChanged);
    //    Q_PROPERTY(qint64      collected_date_end       READ getCollected_date_end   WRITE setCollected_date_end     NOTIFY collected_date_endChanged);
    //    Q_PROPERTY(QString      collected_tee_type        READ getCollected_tee_type  WRITE setCollected_tee_type    NOTIFY collected_tee_typeChanged);





    //    QObject::connect(this,&PictureManager::collected_source_typeChanged,this,&PictureManager::update_selected_picture);
    //  //  QObject::connect(this,&PictureManager::collected_levelChanged,this,&PictureManager::update_selected_picture);
    //    QObject::connect(this,&PictureManager::collected_has_tee_typeChanged,this,&PictureManager::update_selected_picture);
    //    QObject::connect(this,&PictureManager::collected_date_beginChanged,this,&PictureManager::update_selected_picture);
    //    QObject::connect(this,&PictureManager::collected_date_endChanged,this,&PictureManager::update_selected_picture);
    //  //  QObject::connect(this,&PictureManager::collected_choosed_tee_indexsChanged,this,&PictureManager::update_selected_picture);
    // //   QObject::connect(this,&PictureManager::collected_tee_typeChanged,this,&PictureManager::update_selected_picture);


    //    collected_source_type=ImagesTable::PictureSoureType::All;

    //    collected_level = ImagesTable::PictureLevel::All;
    //    collected_has_tee_type = ImagesTable::PictureHasTeeType::All;


    //    QTime t =QTime::currentTime();
    //    int left_hour = 23 - t.hour();
    //    int left_min =59 - t.minute();

    _collected_adult_teeth_indexs = QJsonArray();
    _collected_kid_teeth_indexs = QJsonArray();

    //    collected_date_end = now.addSecs(left_hour * 60 *60 + left_min*60).toMSecsSinceEpoch(); //日期过滤
    //    _collected_tee_type = "adult";
    // _collected_tee_indexs_selecter_image_provider->select(_collected_selected_tee_type);
    drop_signal = false;

}

void PictureManager::imageSaveDCMIMG(const QImage & image,const QString & save_path)
{
    //    ImagesTable * instance = ImagesTable::getInstance();
    //    QString image_path =  QApplication::applicationDirPath() + "/" + instance->query_picture_path(_current_editing_picture_id);
    //    qDebug() << " query_picture_path =========== " << image_path;
    // Load image
    //    QImage image = pic;

    if (image.isNull()) {
        return ;
    }

    // Prepare DICOM dataset
    /******文件头*****/

    DcmFileFormat dcmff;
    OFCondition status;
    DcmMetaInfo *metainfo = dcmff.getMetaInfo();
    char uid[100];
    metainfo->putAndInsertString(DCM_FileMetaInformationVersion,  "us test dcm file");
    metainfo->putAndInsertString(DCM_MediaStorageSOPClassUID, UID_RETIRED_UltrasoundImageStorage);
    metainfo->putAndInsertString(DCM_MediaStorageSOPInstanceUID,    dcmGenerateUniqueIdentifier(uid,SITE_INSTANCE_UID_ROOT));
    metainfo->putAndInsertString(DCM_TransferSyntaxUID, UID_LittleEndianExplicitTransferSyntax);
    metainfo->putAndInsertString(DCM_ImplementationClassUID,"Dicom3.0");
    //数据
    DcmDataset *dataset = dcmff.getDataset();

    // Set DICOM attributes
    dataset->putAndInsertString(DCM_PatientName, "");
    dataset->putAndInsertString(DCM_PatientID, QString::number(_patient_id).toStdString().c_str());
    dataset->putAndInsertString(DCM_StudyInstanceUID, "999.999.2.19941105.112000");
    dataset->putAndInsertString(DCM_SeriesInstanceUID, "999.999.2.19941105.112000.2");
    dataset->putAndInsertString(DCM_PhotometricInterpretation, "RGB");
    dataset->putAndInsertSint32(DCM_SamplesPerPixel, 3);
    dataset->putAndInsertSint32(DCM_Rows, image.height());
    dataset->putAndInsertSint32(DCM_Columns, image.width());
    dataset->putAndInsertSint32(DCM_BitsAllocated, 8);
    dataset->putAndInsertSint32(DCM_BitsStored, 8);
    dataset->putAndInsertSint32(DCM_HighBit, 7);
    dataset->putAndInsertSint32(DCM_PixelRepresentation, 0);

    DcmPixelData* pixelData = new DcmPixelData(DCM_PixelData);
    pixelData->putUint8Array((quint8*)image.bits(), image.sizeInBytes());

    dataset->insert(pixelData, true);
    QString temp  = save_path;
    temp = temp.replace("file:///", "");

    //qDebug() << " save_path ==================================== " << save_path;
    status = dcmff.saveFile(temp.toUtf8().data(), EXS_LittleEndianImplicit,EET_UndefinedLength,EGL_withoutGL);

    return;
}

void PictureManager::imageSaveDCMDIR(QString save_path)
{
    // imageSaveDCMIMG(save_path);
}

QString PictureManager::queryRemainingMemory()
{
    double total_g;
    double free_g;
    QString path = Tools::get_root_path();


    LPCWSTR driver = (LPCWSTR)path.utf16();
    ULARGE_INTEGER free_bytes_avalid, total_bytes, free_bytes;

    if ( !GetDiskFreeSpaceEx(driver, &free_bytes_avalid, &total_bytes, &free_bytes)) {
        total_g = -1;
        free_g = -1;
        return "0";
    }
    // GB
    free_g = free_bytes.QuadPart / 1024.0 / 1024 / 1024;
    total_g = total_bytes.QuadPart / 1024.0 / 1024 / 1024;
    return QString::number(free_g, 'f', 1) + "GB";
}

bool PictureManager::updateImageNote(QString picture_id, QString image_note)
{
    if (ImagesTable::getInstance()->updateImageNote(picture_id, image_note)) {
        QString picture_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_ID_TEXT);
        QString image_note_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Describe_TEXT);
        for(int i = 0;i < _all_data.size();i++)
        {
            QJsonObject & ele = _all_data[i];
            if(ele[picture_key].toString() == picture_id)
            {
                ele[image_note_key] = image_note;
                return _update_picture_property_any_where(picture_id, ele, true, true);
            }
        }
    }
    return false;
}

bool PictureManager::moveImage(QString picture_id, int patient_id)
{
    //废弃
    return false;
    if (ImagesTable::getInstance()->moveImage(picture_id, patient_id)) {
        //移除图片列表的中被转移成功的图片
        _collected_picture_model->remove(picture_id);
        //移除图片编辑的中被转移成功的图片
        _editing_picture_model->remove(picture_id);
        return true;
    }
    return false;
}

QObject *PictureManager::get_today_picture_model()
{
    return _today_picture_model;
}

void PictureManager::save_factory_fix_file(const QJsonArray &files)
{
    auto st = Settings::get_instance();
    if(files.size() != 2){
        show_message("",st->tran("数量不对，请选择2个文件"));
        return;
    }



    QString file1_source = files.at(0).toString();
    file1_source = file1_source.replace("file:///", "");

    QString file2_source = files.at(1).toString();
    file2_source = file2_source.replace("file:///", "");




    QString dark_file,light_file;
    if(file1_source.contains("dark"))
        dark_file = file1_source;
    if(file2_source.contains("dark"))
        dark_file = file2_source;

    if(file1_source.contains("light"))
        light_file = file1_source;
    if(file2_source.contains("light"))
        light_file = file2_source;
    qDebug() << light_file <<dark_file << file1_source <<file2_source <<files;

    if(light_file.isNull() || dark_file.isNull())
    {
        show_message("",st->tran("文件名字未包含\"dark\",或者\"light\"字段，导入失败"));
        return;
    }
    QString d_name = dark_file.split("/").last();
    QString l_name = light_file.split("/").last();
    QFile d(dark_file);
    d.open(QIODevice::ReadOnly);
    QString  d_save_file =   Configure::data_store_root_dir + "/factory_fixed/"+d_name;
    QString  l_save_file =   Configure::data_store_root_dir + "/factory_fixed/"+l_name;
    QFile d_(d_save_file);
    d_.open(QIODevice::WriteOnly);
    bool ok0 = d_.write(d.readAll());
    d_.close();

    QFile l(light_file);
    l.open(QIODevice::ReadOnly);
    QFile l_(l_save_file);
    l_.open(QIODevice::WriteOnly);
    bool ok1 =  l_.write(l.readAll());
    l_.close();
    if(ok0 && ok1)
    {
        show_message("",st->tran("成功"));
    }
    else
    {
        show_message("",st->tran("失败"));
    }

}

void PictureManager::reset_factory_fix_file()
{
    reset_factor_image();


}

