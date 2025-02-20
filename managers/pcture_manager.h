#ifndef PICTUREMANAGER_H
#define PICTUREMANAGER_H
#include <QObject>
#include "c++/models/eidt_page_editing_picture_model.h"
#include "c++/models/picture_model.h"
#include <QDateTime>
#include <QTime>
#include "c++/image_providers/tee_indexs_selecter.h"
#include"c++/image_providers/editing_image_provider.h"
#include "c++/devices/usb_reader.h"
#include<QQueue>
#include<QSettings>
#include"c++/define_tool.h"
#include"../image_providers/fixed_image.h"
#include"../models/today_pic_model.h"
#define TodayCollectedPictureModel Today_Pic_Model
/**
 * @brief The PatientAllPictureManager class
 * 所有图片管理类
 */
class PictureManager : public QObject{
    Q_OBJECT
    Q_PROPERTY(int      collected_source_type        READ getCollected_source_type   WRITE setCollected_source_type   NOTIFY collected_source_typeChanged);
    Q_PROPERTY(int      collected_level        READ getCollected_level  WRITE setCollected_level     NOTIFY collected_levelChanged);
    Q_PROPERTY(int      collected_has_tee_type        READ getCollected_has_tee_type  WRITE setCollected_has_tee_type     NOTIFY collected_has_tee_typeChanged);
    Q_PROPERTY(qint64      collected_date_begin        READ getCollected_date_begin  WRITE setCollected_date_begin     NOTIFY collected_date_beginChanged);
    Q_PROPERTY(qint64      collected_date_end       READ getCollected_date_end   WRITE setCollected_date_end     NOTIFY collected_date_endChanged);
    Q_PROPERTY(QString      collected_tee_type        READ getCollected_tee_type  WRITE setCollected_tee_type    NOTIFY collected_tee_typeChanged);


    //  Q_PROPERTY(QJsonArray collected_choosed_tee_indexs READ getCollected_choosed_tee_indexs WRITE setCollected_choosed_tee_indexs NOTIFY collected_choosed_tee_indexsChanged);


    Q_PROPERTY(QString      collecting_tee_type     READ getCollecting_tee_type  WRITE setCollecting_tee_type    NOTIFY collecting_tee_typeChanged);
  //  Q_PROPERTY(QJsonArray   collecting_tee_indexs        READ getCollecting_tee_indexs  WRITE setCollecting_tee_indexs    NOTIFY collecting_tee_indexsChanged);
    //Q_PROPERTY(QString      collecting_tee_type_picture  ); //url

   Q_PROPERTY(PictureModel * pictureModel    READ getPictureModel    CONSTANT  )
  // Q_PROPERTY(TodayCollectedPictureModel * today_picture_model    READ getToday_picture_model    CONSTANT)
   Q_PROPERTY(EditingPictureModel * editing_picture_model    READ get_editing_model    CONSTANT)


    Q_PROPERTY(QString collecting_lastest_picture_url READ getCollecting_lastest_picture_url WRITE setCollecting_lastest_picture_url    NOTIFY collecting_lastest_picture_urlChanged);
        //今天最后采集的图片链接
    Q_PROPERTY(int  editing_count    READ getEditing_count     WRITE setEediting_count    NOTIFY editing_countChanged)
    Q_PROPERTY(QString  ver_shader  READ getVer_shader   CONSTANT  );
    Q_PROPERTY(QString  fram_shader  READ getFram_shader   CONSTANT  );
    Q_PROPERTY(int  sreen_width  READ getSreen_width  CONSTANT  );
    Q_PROPERTY(int  sreen_height  READ getSreen_height  CONSTANT  );
    Q_PROPERTY(QString current_editing_picture_id READ getCurrent_editing_picture_id WRITE setCurrent_editing_picture_id    NOTIFY current_editing_picture_idChanged);
    //当前编辑的图片id

    Q_PROPERTY(int current_editing_picture_state READ getCurrent_editing_picture_state WRITE setCurrent_editing_picture_state   NOTIFY current_editing_picture_stateChanged);
    //当前编辑图片的状态，可能是原图或者最后状态
    Q_PROPERTY(bool can_undo READ getCan_undo WRITE setCan_undo  NOTIFY can_undoChanged);
    Q_PROPERTY(bool can_redo READ getCan_redo WRITE setCan_redo  NOTIFY can_redoChanged);
    Q_PROPERTY(int expose_second_index READ getExpose_second_index WRITE setExpose_second_index NOTIFY expose_second_indexChanged); //记住用户设置的曝光时间
    Q_PROPERTY(QString expose_second_value READ getExpose_second_value WRITE setExpose_second_value NOTIFY expose_second_valueChanged); //记住用户设置的曝光时间
    Q_PROPERTY(QString expose_cm READ getExpose_cm WRITE setExpose_cm NOTIFY expose_cmChanged); //记住用户设置的曝光距离
    Q_PROPERTY(QString  voltage_v READ getVoltage_v WRITE setVoltage_v NOTIFY voltage_vChanged); //记住用户设置的电压
    Q_PROPERTY(QString electricity_a READ getElectricity_a WRITE setElectricity_a NOTIFY electricity_aChanged); //记住用户设置的电流
    Q_PROPERTY(int source_index READ getSource_index WRITE setSource_index NOTIFY source_indexChanged) //0是扫描仪，1是导入,2是usb传感器,记住用户上传使用的文件来源

//    Q_PROPERTY(int source_index READ getSource_index WRITE setSource_index NOTIFY source_indexChanged) //0是扫描仪，1是导入,2是usb传感器,记住用户上传使用的文件来源
    Q_PROPERTY(bool show_transformation_bar READ getShow_transformation_bar WRITE setShow_transformation_bar NOTIFY show_transformation_barChanged) //变换栏
    Q_PROPERTY(bool show_fix_image_bar READ getShow_fix_image_bar WRITE setShow_fix_image_bar NOTIFY show_fix_image_barChanged) //影像修复栏
    Q_PROPERTY(bool show_filter_bar READ getShow_filter_bar WRITE setShow_filter_bar NOTIFY show_filter_barChanged) //滤镜栏
    Q_PROPERTY(bool show_color_space_bar READ getShow_color_space_bar WRITE setShow_color_space_bar NOTIFY show_color_space_barChanged) //色域调节栏
    Q_PROPERTY(bool show_meter_bar READ getShow_meter_bar WRITE setShow_meter_bar NOTIFY show_meter_barChanged) //测量栏
    Q_PROPERTY(bool show_rui_hua_bar READ getShow_rui_hua_bar WRITE setShow_rui_hua_bar NOTIFY show_rui_hua_barChanged) //锐化栏
    Q_PROPERTY(bool show_mark_bar READ getShow_mark_bar WRITE setShow_mark_bar NOTIFY show_mark_barChanged) //标注栏
    Q_PROPERTY(bool show_mark_manager_bar READ getShow_mark_manager_bar WRITE setShow_mark_manager_bar NOTIFY show_mark_manager_barChanged) //标注管理栏
    DEFINE_PRO_NO_AUTO_GET_SET(QJsonObject,last_pic_info,QJsonObject())
    DEFINE_PRO_NO_AUTO_GET_SET(bool,usb_connected,false)
    DEFINE_PRO_NO_AUTO_GET_SET(bool,fixed_mode_starting,false)
    //图标校准模式,出图改成直接出图，点击开始的时候旧改成 true,且界面根据下面的标准做提示，比如是 cuurrent_pic_is_lighting 提示拍照,或者直接出图
    DEFINE_PRO_NO_AUTO_GET_SET(bool,cuurrent_pic_is_lighting,true) //需要拍照模式
    DEFINE_PRO_NO_AUTO_GET_SET(bool,show_fixed_page,false) //需要拍照模式

    DEFINE_PRO_NO_AUTO_GET_SET(bool,fixed_new_version,true) //开启图片校准
    DEFINE_PRO_NO_AUTO_GET_SET(bool,enable_scale_0_65535,true) //开启图片缩放

    DEFINE_PRO_NO_AUTO_GET_SET(int,avager_ct_value_dark,0) //开启图片缩放
    DEFINE_PRO_NO_AUTO_GET_SET(int,avager_ct_value_light,0) //开启图片缩放

    DEFINE_PRO_NO_AUTO_GET_SET(bool,usb_reading,false) //开启图片缩放
    DEFINE_PRO_NO_AUTO_GET_SET(QString,dark_image_url,"image://fixed_images/dark0") //开启图片缩放
    DEFINE_PRO_NO_AUTO_GET_SET(QString,light_image_url,"image://fixed_images/light0") //开启图片缩放
    DEFINE_PRO_NO_AUTO_GET_SET(int,fixed_state_index,0) //开启图片缩放
    DEFINE_PRO_NO_AUTO_GET_SET(QString,usb_version,"") //固件版本
    DEFINE_PRO_NO_AUTO_GET_SET(QString,usb_ser_num,"") //序列号
    DEFINE_PRO_NO_AUTO_GET_SET(int,edit_grid_index,1) //格子

    DEFINE_PRO_NO_AUTO_GET_SET(bool,dont_show_usb_info,false) //弹窗显示usb插入，和校准提示
    const bool DEBUG = true;
    const QString TAG = "图片管理类";
private:
    struct FillterForPatientPageCollectedPicture{ //患者页面已收集的图片过滤器
        FillterForPatientPageCollectedPicture();
        void reinit();
        ImagesTable::PictureSoureType collected_source_type;
        ImagesTable::PictureLevel collected_level;
        ImagesTable::PictureHasTeeType collected_has_tee_type;
        qint64 collected_date_begin,collected_date_end; //日期过滤
      //  QString _collected_selected_tee_type = "adult"; //收集后的牙齿类型
     //  TeeIndexsSelecterImageProvider * _collected_tee_indexs_selecter_image_provider; //收集后的牙齿选择器

        QJsonArray _collected_adult_teeth_indexs; //收集中的成人牙齿选中状态
        QJsonArray _collected_kid_teeth_indexs; //收集者小孩牙齿选择状态
        QJsonArray _collected_adult_teeth_text; //收集中的成人牙齿选中编号
        QJsonArray _collected_kid_teeth_text; //收集者小孩牙齿选择编号
        QString _collected_tee_type = "adult"; //收集中的牙齿类型 adult和baby
      //  QString _collected_tee_from = "Palmer"; //收集中的牙齿类型 adult和baby
        bool drop_signal;
    };
    struct Picture_Visible_Propertys
    {
        QString _picture_id;
        QJsonObject _current_property;
        int stack;
    };
    enum class LastDataHandle
    {
        Save,Undo,Redo
    };


public:
    Q_INVOKABLE void reset_collected_page_picture_filter(); //重置过滤器

    int getCollected_source_type() const;
    void setCollected_source_type(int value);

    int getCollected_level() const;
    void setCollected_level(int value);

    int getCollected_has_tee_type() const;
    void setCollected_has_tee_type(int value);

    qint64 getCollected_date_begin() const;
    void setCollected_date_begin(const qint64 &value);

    qint64 getCollected_date_end() const;
    void setCollected_date_end(const qint64 &value);

    QString getCollected_tee_type() const;
    void setCollected_tee_type(const QString &collected_tee_type);

    QString getCollecting_tee_type() const;
    void setCollecting_tee_type(const QString &collecting_tee_type);


    int getEditing_count() const;
    void setEediting_count(int value);

  //  TeeIndexsSelecterImageProvider * get_collecting_tee_indexs_select_iamge_for_collecting_provider(); //采集中的牙齿选择图片提供者
 //   TeeIndexsSelecterImageProvider * get_collected_tee_indexs_select_iamge_for_collecting_provider(); //采集后的牙齿选择图片提供者
   // TeeIndexsSelecterImageProvider * get_collected_tee_indexs_select_iamge_for_picture_info_provider(); //图片信息的牙齿选择图片提供者
    EditingImageProvider * get_editing_iamge_provider();
    Fixed_Image * get_fixed_image_provider(){return  _fixed_image;}
    QString getVer_shader();
    QString getFram_shader();
 //   PictureModel * get_picture_model(); //获得采集后的图片model
//    QJsonArray getCollected_choosed_tee_indexs() const;
//    void setCollected_choosed_tee_indexs(const QJsonArray &value);

signals:
    void collected_source_typeChanged();//通知更新model
    void collected_levelChanged();//通知更新model
    void collected_has_tee_typeChanged();//通知更新model
    void collected_date_beginChanged();//通知更新model
    void collected_date_endChanged();//通知更新model
    void collected_choosed_tee_indexsChanged();//通知更新model
    void collected_choosed_tee_textChanged();//通知更新model
    void collecting_tee_typeChanged();//需要更新图片
    void collected_tee_typeChanged(); //需要更新图片

    void collecting_picture_url_change(const QString &url);//通知更新图片
    void collected_picture_url_change(const QString &url);//通知更新图片
    void picture_info_url_change(const QString &url);//通知更新图片
    void picture_info_tee_indexs_change(const QString & new_tee_indexs);
    void collecting_lastest_picture_urlChanged(); //通知更新最新的采集图片url
    void current_editing_picture_idChanged(); //

    void show_message(const QString &tag,const QString & message);
    void editing_countChanged();
    void aync_new_value_to_eidting_page_right_tool_bar(const QJsonObject & new_values); //同步新属性到ui
    void aync_apply_filter_to_eidting_page_right_tool_bar(const QJsonObject & new_values); //同步新属性到ui
    //滤镜过后需要刷新一些参数到右边
    void not_found_usb();
    void usb_timeout();
    void got_usb_picture();
    //
    void reset_fillter_ui(); //通知ui重置

    void begin_export_pictures(const QJsonArray & ids);
    void current_editing_picture_stateChanged();
    void can_undoChanged();
    void can_redoChanged();
    void got_report_image(const QImage &pic, const QJsonObject &info);
    void expose_second_indexChanged();
    void expose_second_valueChanged();
    void expose_cmChanged();
    void voltage_vChanged();
    void electricity_aChanged();
    void source_indexChanged();

    void show_transformation_barChanged(); //变换栏
    void show_fix_image_barChanged(); //影像修复栏
    void show_filter_barChanged(); //滤镜栏
    void show_color_space_barChanged(); //色域调节栏
    void show_meter_barChanged(); //测量栏
    void show_rui_hua_barChanged(); //锐化栏
    void show_mark_barChanged(); //标注栏
    void show_mark_manager_barChanged(); //标注管理栏


    void max_image_view_index_changed(int index); //最大化后的图片索引改变
    DEFINE_SIG(last_pic_info)
    DEFINE_SIG(usb_connected)
    DEFINE_SIG(fixed_mode_starting) //图标校准模式,出图改成直接出图
    DEFINE_SIG(cuurrent_pic_is_lighting) //需要拍照模式
    DEFINE_SIG(show_fixed_page)
    DEFINE_SIG(fixed_new_version)
    DEFINE_SIG(enable_scale_0_65535)
    DEFINE_SIG(avager_ct_value_dark)
    DEFINE_SIG(usb_reading)
    DEFINE_SIG(dark_image_url) //开启图片缩放
    DEFINE_SIG(light_image_url) //开启图片缩放
    DEFINE_SIG(avager_ct_value_light) //开启图片缩放

    DEFINE_SIG(fixed_state_index) //开启图片缩放
    void new_state(int state,bool ok,const QString & info);
    DEFINE_SIG(usb_version) //固件版本
    DEFINE_SIG(usb_ser_num) //序列号
    DEFINE_SIG(edit_grid_index)


    void no_usb_found();
    void no_fixed_file_found();
    DEFINE_SIG(dont_show_usb_info)
private slots:
    void update_selected_picture();//更新model
public:
   // Q_INVOKABLE void setCollecting_tee_from(const QString &collecting_tee_from);
    PictureManager(QObject *parent = nullptr);
    Q_INVOKABLE void load_pictures(qint64 patient_id); //加载图片数据
    Q_INVOKABLE void da_task(const QString & task, const QJsonObject  & info); //用户事件
    Q_INVOKABLE void mouse_event(float x, float y,const QString & action,const QString & doing);//action是鼠标动作("m","c"), doing是为了哪个过滤做选择，可能的参数是 collecting,和 collected

   // Q_INVOKABLE void init_pircture_info_iamge_provider(const QString & tee_type,const QString & tee_indexs); //初始化牙齿选择的图片提供者,图片一定会刷新
   // Q_INVOKABLE QString switch_tee_type_for_picture_info(const QString &new_type); //选择牙齿型号,可能是成人或者婴儿,图片一定会刷新

    Q_INVOKABLE void on_import_file(const QJsonArray &files); //触发导入文件
    Q_INVOKABLE void delete_one_picuture(const QString &picture_id, const QString &path); //删除一个图片
    Q_INVOKABLE void checked_export_or_delete(bool checked); //更改一个checked字段的值
    Q_INVOKABLE void delete_checked_pictures(); //删除checked后的所有图片
    Q_INVOKABLE void export_checked_pictures(const QString &des_dir); //导出checked后的所有图片
    Q_INVOKABLE void export_one_picture(const QString & picture_id, const QString & target);//导出一个图片
    Q_INVOKABLE void update_picture_from_picture_info_page(const QString &picture_id, const QJsonObject & info); //右击图片弹出的图片信息，然后点击 save后进行更新一些属性
   //  void update_picture_property_any_where(const QString &picture_id, const QJsonObject & new_property);
    Q_INVOKABLE void only_update_property_to_model(const QString &picture_id, const QJsonObject & new_property);//不会存到数据库
    bool _update_picture_property_any_where(const QString &picture_id, const QJsonObject & new_property, bool test_filter_, bool save_to_datebase);//所有更新属性入口函数
 //   void update_property_to_data_base(const QString &picture_id, const QJsonObject & new_property);
    Q_INVOKABLE void update_one_visible_value(const QString &picture_id, const QString & key, const QJsonValue & value);//更新一个可见属性的值，
    Q_INVOKABLE void update_mm_per_pixel_value(const QString &picture_id, const QString & key, const QJsonValue & value); //更新像素间距
    Q_INVOKABLE void update_scale_factor_to_model(const QString & picture_id,float factor);

    Q_INVOKABLE void add_one_picture_to_editing_picture_model(const QString & picture_id); //双击采集中页面的图片,或者双击历史图片,就会触发此函数
    Q_INVOKABLE void add_checked_pictures_to_editing_picture_model();//多选后批量追加进来编辑中model
    Q_INVOKABLE void remove_one_picture_on_editing_picture_model(const QString & picture_id); //移除编辑中model一个图片
    Q_INVOKABLE void remove_all_picture_on_editing_picture_model(); //移除所有编辑列表的model
    Q_INVOKABLE QString get_lastest_picture_id(); //获得最后采集的图片id
    Q_INVOKABLE QJsonObject get_params(const QString & picture_id);//获得一个图片的信息
    Q_INVOKABLE void apply_filter(const QString & picture_id,int filter);

    Q_INVOKABLE bool start_read_usb(int fillter);
    Q_INVOKABLE bool stop_read_usb();
      Q_INVOKABLE void use_lastest_usb_image();
    Q_INVOKABLE bool on_image_state_changed();//const QString & id,int new_state);
    Q_INVOKABLE void on_usm_value_change(const QString & id, int value);

    Q_INVOKABLE QJsonObject undo(); //  撤销
    Q_INVOKABLE QJsonObject redo(); //反撤销
    Q_INVOKABLE void save_current_state(); //保存当前状态
    Q_INVOKABLE void clear_all_states(); //现在所有图片都用实时效果图了
    Q_INVOKABLE void set_editing_picture_id(const QString & picture_id);//, int state);

    Q_INVOKABLE void set_collecting_adult_teeth_indexs(const QJsonArray & indexs); //设置收集中的图片牙齿类型,成人
    Q_INVOKABLE void set_collecting_kit_teeth_indexs(const QJsonArray & indexs); //设置收集中的图片牙齿类型，小孩
    Q_INVOKABLE void set_collecting_adult_teeth_text(const QJsonArray & text); //设置收集中的图片牙齿类型,成人
    Q_INVOKABLE void set_collecting_kit_teeth_text(const QJsonArray & text); //设置收集中的图片牙齿类型，小孩
    Q_INVOKABLE void set_collected_adult_teeth_indexs(const QJsonArray & indexs); //设置采集后所有图片过滤条件牙齿类型，成人
    Q_INVOKABLE void set_collected_kit_teeth_indexs(const QJsonArray & indexs); //设计采集后所有图片过滤条件牙齿类型，小孩
    Q_INVOKABLE void set_collected_adult_teeth_text(const QJsonArray & indexs); //设置采集后所有图片过滤条件牙齿类型，成人
    Q_INVOKABLE void set_collected_kit_teeth_text(const QJsonArray & indexs); //设计采集后所有图片过滤条件牙齿类型，小孩
    Q_INVOKABLE void save_image(const QImage & pic,const QJsonObject & info);
    Q_INVOKABLE void save_default_visible_params(const QJsonObject &params);
    Q_INVOKABLE QString reportImageCopy();
    Q_INVOKABLE void reverseColorPicture(const QString & picture_id, const QString &key);

    Q_INVOKABLE void imageSaveDCMIMG(const QImage & pic, const QString &save_path);
    Q_INVOKABLE void imageSaveDCMDIR(QString save_path);

    Q_INVOKABLE QString queryRemainingMemory();

    Q_INVOKABLE bool updateImageNote(QString picture_id, QString image_note);
    Q_INVOKABLE bool moveImage(QString picture_id, int patient_id);
    Q_INVOKABLE QObject * get_today_picture_model();
    Q_INVOKABLE  void save_factory_fix_file(const QJsonArray &files);
    Q_INVOKABLE  void reset_factory_fix_file();
    bool get_final_image(const QString &id);//,QImage & out);

    QJsonObject on_new_image(const QString & path, const QString &last_state_path, const QString &pillar_path, int width, int height, qint64 pts, ImagesTable::PictureSoureType source_type, int w, int h, float hor_pix_space_mm, float ver_pix_space_mm); //新图片

    PictureModel * getPictureModel();
   // TodayCollectedPictureModel * getToday_picture_model();
    EditingPictureModel * get_editing_model();
    QString getCollecting_lastest_picture_url() const;

    void setCollecting_lastest_picture_url(const QString &collecting_lastest_picture_url);//设置最新的图片url
    void setLastest_picture_id(const QString &lastest_picture_id); //设置最新的图片id

    void on_delete_this_paitent(qint64 patient_id);
    Q_INVOKABLE  void clear_fixed_image();
    Q_INVOKABLE  void get_usb_info();
    QString get_dark_pic_dir();
    QString get_light_pic_dir();
    QString get_factory_dark_pic_dir();
    QString get_factory_light_pic_dir();
    QString get_usb_dark_pic_dir(const QString &root_dir);
    QString get_usb_light_pic_dir(const QString & root_dir);
    bool  save_dark_image(const QByteArray & data);
    bool save_light_image(const QByteArray & data);
    QByteArray load_dark_image();
    QByteArray load_light_image();
    void reset_factor_image();
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!收到图片时，应该都把日期筛选改成今天的日期，否则看不到图片
    int getSreen_width() const;
    void setSreen_width(int sreen_width);

    int getSreen_height() const;
    void setSreen_height(int sreen_height);

    QString getCurrent_editing_picture_id() const;
    void setCurrent_editing_picture_id(const QString &current_editing_picture_id);

    int getCurrent_editing_picture_state() const;
    void setCurrent_editing_picture_state(int current_editing_picture_state);

    bool getCan_undo() const;
    void setCan_undo(bool can_undo);

    bool getCan_redo() const;
    void setCan_redo(bool can_redo);

    int getExpose_second_index() const;
    void setExpose_second_index(int expose_second_index);

    QString getExpose_second_value() const;
    void setExpose_second_value(QString expose_second_value);

    QString getExpose_cm() const;
    void setExpose_cm(const QString &expose_cm);

    QString getVoltage_v() const;
    void setVoltage_v(const QString &voltage_v);

    QString getElectricity_a() const;
    void setElectricity_a(const QString &electricity_a);

    int getSource_index() const;
    void setSource_index(int source_index);

    bool getShow_transformation_bar() const;
    void setShow_transformation_bar(bool show_transformation_bar);

    bool getShow_fix_image_bar() const;
    void setShow_fix_image_bar(bool show_fix_image_bar);

    bool getShow_filter_bar() const;
    void setShow_filter_bar(bool show_filter_bar);

    bool getShow_color_space_bar() const;
    void setShow_color_space_bar(bool show_color_space_bar);

    bool getShow_meter_bar() const;
    void setShow_meter_bar(bool show_meter_bar);

    bool getShow_rui_hua_bar() const;
    void setShow_rui_hua_bar(bool show_rui_hua_bar);

    bool getShow_mark_bar() const;
    void setShow_mark_bar(bool show_mark_bar);

    bool getShow_mark_manager_bar() const;
    void setShow_mark_manager_bar(bool show_mark_manager_bar);


private:
    void reset(qint64 new_patient_id); //重置一些字段
    static bool test_filter(const QJsonObject & picture, ImagesTable::PictureSoureType picture_soure_type,ImagesTable::PictureLevel picture_level,ImagesTable::PictureHasTeeType picture_has_tee_type,qint64 begin,qint64 end,const QJsonArray & tee_indexs, const QJsonArray & tee_text, const QString &tee_type); //过滤条件
    bool get_visible_value(const QString & pirture_id, QJsonObject & out);
    void clear_usb_reader(UsbReader * usb);
    PictureModel * _collected_picture_model; //收集到的图片
    TodayCollectedPictureModel * _today_picture_model;
    EditingPictureModel * _editing_picture_model;
    qint64 _today_pts;
    QList<QJsonObject> _all_data;
    qint64 _patient_id; //当前病人id
  //  TeeIndexsSelecterImageProvider * _tee_indexs_selecter_provider_for_collecting_image; //收集中的牙齿图片选择器，图片提供者
    QJsonArray _collecting_adult_teeth_indexs; //收集中的成人牙齿选中状态
    QJsonArray _collecting_adult_teeth_text; //收集中的成人牙齿选中牙齿型号
    QJsonArray _collecting_kid_teeth_indexs; //收集者小孩牙齿选择状态
    QJsonArray _collecting_kid_teeth_text; //收集者小孩牙齿选择的牙齿型号
    QString _collecting_tee_type = "adult"; //收集中的牙齿类型 adult和baby
   // QString _collecting_tee_from = "Palmer"; //牙齿类型

   // TeeIndexsSelecterImageProvider * _tee_indexs_selecter_provider_for_picture_info; //图片信息中的牙齿选择器，图片提供者

    FillterForPatientPageCollectedPicture * _fillter_for_patient_page_collected_picture;  //收集后的图片
    QString _collecting_lastest_picture_url;//最新的采集图片url
    QString _lastest_picture_id = "-1"; //最后的图片id
    int _editing_count= 0;
    EditingImageProvider * _editing_iamge_provider; //编辑中的图片提供者
    Fixed_Image * _fixed_image;
    QString  _ver_shader;
    QString _fram_shader;
    UsbReader * _usb_reader = nullptr;
    QList<Picture_Visible_Propertys> _picture_old_visible_property; //历史轨迹
   // QList<Picture_Visible_Propertys> _picture_old_visible_property_redo; //可以反撤销的轨迹
   // LastDataHandle _last_data_handle = LastDataHandle::Save; //最后操作数据的方向
    int _current_visible_property_index = 0;
    //追加数据是lefting,撤销是righting,反撤销是righting
    QString _current_editing_picture_id = "-1"; //编辑中的图片id
   // RuiHuaThread * _rui_hua_thread;
    ImageState _current_image_state;
    QJsonObject _change_state_property;
    bool _change_filter_then_update_iamge_state = false;
   int  _sreen_width;
    int  _sreen_height;
  //  QSettings settings;
    bool _can_undo = false;
    bool _can_redo = false;
    int _expose_second_index;
    QString _expose_second_value;
    QString _expose_cm;
    QString _voltage_v;
    QString _electricity_a;
    int _source_index = 1;


    bool _show_transformation_bar; //变换栏
    bool _show_fix_image_bar; //影像修复栏
    bool _show_filter_bar; //滤镜栏
    bool _show_color_space_bar;//色域调节栏
    bool _show_meter_bar;//测量栏
    bool _show_rui_hua_bar;//锐化栏
    bool _show_mark_bar;//标注栏
    bool _show_mark_manager_bar;//标注栏

};
#endif // PICTUREMANAGER_H
