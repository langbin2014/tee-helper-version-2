#ifndef PICTURELINEMANAGER_H
#define PICTURELINEMANAGER_H

/**
 * @brief The PictureLineManager class,图片的线条管理类
 */
#include<QObject>
#include"c++/table/line_table.h"
#include<map>
#include<QList>
#include<QSettings>
#include"c++/table/images_table.h"
#include <QPainterPath>
#include"c++/line_type.h"
class LineManager : public QObject
{
    const QString TAG = "线管理类";
    const bool DEBUG = false;
    Q_OBJECT
    Q_PROPERTY(int      line_task        READ getLine_task    WRITE setLine_task    NOTIFY line_taskChanged);
    Q_PROPERTY(int      mouse_task        READ getMouse_task    WRITE setMouse_task    NOTIFY mouse_taskChanged);
    Q_PROPERTY(bool      show_cross_mouse        READ getShow_cross_mouse    WRITE setShow_cross_mouse    NOTIFY show_cross_mouseChanged);
    Q_PROPERTY(bool hor_mirror        READ getHor_mirror    WRITE setHor_mirror NOTIFY hor_mirrorChanged)
    Q_PROPERTY(bool ver_mirror        READ getVer_mirror    WRITE setVer_mirror NOTIFY ver_mirrorChanged)
    Q_PROPERTY(float scale_factor        READ getScale_factor    WRITE setScale_factor NOTIFY scale_factorChanged)
    Q_PROPERTY(int rotate_factor        READ getRotate_factor    WRITE setRotate_factor NOTIFY rotate_factorChanged)

    Q_PROPERTY(qint16 current_picture_width        READ getCurrent_picture_width  WRITE setCurrent_picture_width NOTIFY current_picture_widthChanged)
    Q_PROPERTY(qint16 current_picture_height        READ getCurrent_picture_height   WRITE setCurrent_picture_height NOTIFY current_picture_heightChanged)

    Q_PROPERTY(int line_unit  READ getLine_unit  WRITE setLine_unit NOTIFY line_unitChanged)
    Q_PROPERTY(bool show_rule  READ getShow_rule  WRITE setShow_rule NOTIFY show_ruleChanged)

    Q_PROPERTY(float hor_per_pixel_mm  READ getHor_per_pixel_mm   WRITE setHor_per_pixel_mm NOTIFY hor_per_pixel_mmChanged)
    Q_PROPERTY(float ver_per_pixel_mm  READ getVer_per_pixel_mm    WRITE setVer_per_pixel_mm  NOTIFY ver_per_pixel_mmChanged)
    Q_PROPERTY(qint64 checked_line_id  READ getChecked_line_id   WRITE setChecked_line_id NOTIFY checked_line_idChanged)
    Q_PROPERTY(QString  current_line_color READ getCurrent_line_color   WRITE setCurrent_line_color NOTIFY current_line_colorChanged)
    Q_PROPERTY(QString  current_angle_color READ getCurrent_angle_color   WRITE setCurrent_angle_color NOTIFY current_angle_colorChanged)
    Q_PROPERTY(QString  current_pen_color READ getCurrent_pen_color   WRITE setCurrent_pen_color NOTIFY current_pen_colorChanged)

    Q_PROPERTY(bool line_undo READ getLine_undo WRITE setLine_undo  NOTIFY line_undoChanged);
    Q_PROPERTY(bool line_redo READ getLine_redo WRITE setLine_redo  NOTIFY line_redoChanged);
public:
    enum class MouseTask{
        NewLine,
        ChangePoint,
        MoveLine,
        NoMouseTask
    };

    enum  MouseEvent{
        Pressed,
        MoveOnPressed,
        MoveNoPressed,
        Release,
        Clicked
    };
    enum class LineTask{
        Line,   //直线
        Angle,          //角度
        NoLineTask,
        Pen            //画笔
    };

    enum  Operate{ //新增
        Insert, //新增
        Delete //新增
    };

    LineManager(QObject * parent = nullptr);
    static LineManager * get_instance();

    std::shared_ptr<std::list<std::shared_ptr<LineTypesNS::Line>>> get_lines(const QString & picture_id);
    int getLine_task() const;
    void setLine_task(int line_task);
    int getMouse_task() const;
    void setMouse_task(int mouse_task);
    std::shared_ptr<LineTypesNS::Line> get_current_task_line();

    Q_INVOKABLE QString get_picture_id(){
        return _current_picture_id;
    }
    Q_INVOKABLE void load_patien_all_picture_lines(qint64 patient_id);//加载病人所有线条
    Q_INVOKABLE void checked_to_picture(const QString & picture_id);  //当前编辑图片
    Q_INVOKABLE bool mouse_event(const QString &picture_id, qint16 x, qint16 y, int event);
    Q_INVOKABLE bool end_new_line(); //结束画线
    Q_INVOKABLE void end_change_point(); //结束移动点
    Q_INVOKABLE void end_move_line(); //结束移动线
    Q_INVOKABLE void empty_checked_line(); //无选择线条
    Q_INVOKABLE QJsonArray get_lines(); //获得所有线条
    Q_INVOKABLE void switch_this_line_id(qint64 id); //点击线条
    Q_INVOKABLE void delete_one_line(qint64 id); //删除线条
    Q_INVOKABLE void update_line_color(qint64 id, const QString & new_color); //更新线条颜色

    Q_INVOKABLE bool get_fix(){
        return _need_fix;
    }
    Q_INVOKABLE void undo();//撤销 //新增
    Q_INVOKABLE void reverseUndo();//反撤销 //新增
    Q_INVOKABLE void clearAll();//清空所有线 //新增

    bool getShow_cross_mouse() const;
    void setShow_cross_mouse(bool show_cross_mouse);

    bool getHor_mirror() const;
    void setHor_mirror(bool hor_mirror);

    float getScale_factor() const;
    void setScale_factor(float scale_factor);

    bool getVer_mirror() const;
    void setVer_mirror(bool ver_mirror);

    int getRotate_factor() const;
    void setRotate_factor(int rotate_factor);

    qint16 getCurrent_picture_width() const;
    void setCurrent_picture_width(const qint16 &current_picture_width);

    qint16 getCurrent_picture_height() const;
    void setCurrent_picture_height(const qint16 &current_picture_height);

    int getLine_unit() const;
    void setLine_unit(int line_unit);

    bool getShow_rule() const;
    void setShow_rule(bool show_rule);

    float getHor_per_pixel_mm() const;
    void setHor_per_pixel_mm(float hor_per_pixel_mm);

    float getVer_per_pixel_mm() const;
    void setVer_per_pixel_mm(float ver_per_pixel_mm);

    qint64 getChecked_line_id() const;
    void setChecked_line_id(const qint64 &checked_line_id);

    void remove_this_patient_all_lines(qint64 patient_id); //新增
    QString getCurrent_line_color() const; //新增
    void setCurrent_line_color(const QString &current_line_color); //新增

    QString getCurrent_angle_color() const; //新增
    void setCurrent_angle_color(const QString &currennt_angle_color); //新增

    QString getCurrent_pen_color() const;  //新增
    void setCurrent_pen_color(const QString &current_pen_color); //新增

    void getPainterPath(QPainterPath &painter_path); //新增

    std::shared_ptr<LineTypesNS::Line> getCurrentTaskPen(); //新增

    bool saveNewPenData(); //新增

    bool getLine_undo();//新增
    bool getLine_redo();//新增

    void setLine_undo(bool undo); //新增
    void setLine_redo(bool redo); //新增
signals:
    void line_taskChanged();
    void mouse_taskChanged();
    void show_cross_mouseChanged();
    void hor_mirrorChanged();
    void ver_mirrorChanged();
    void scale_factorChanged();
    void rotate_factorChanged();
    void current_picture_widthChanged();
    void current_picture_heightChanged();
    void line_unitChanged();
    void show_ruleChanged();
    void hor_per_pixel_mmChanged();
    void ver_per_pixel_mmChanged();
    void checked_line_idChanged();
    void current_line_colorChanged(); //新增
    void current_angle_colorChanged(); //新增
    void current_pen_colorChanged(); //新增


    void new_line(const QJsonObject &info); //新添加一条线
    void line_display_text_changed(qint64 _id,const QString & new_text);//长度和角度,颜色改变
    void repaint_checked_line(const QString & _picture_id); //点击右边的线列表， 通知此线图片重新画线

    void line_undoChanged(); //新增
    void line_redoChanged(); //新增
private slots:
    void update_all_line_display_text();
private:
    // void reset_field_for_one_current_picture();
    bool mouse_move_on_pressed(qint16 x, qint16 y);
    bool mouse_move_no_pressed(qint16 x, qint16 y);
    bool mouse_clicked(qint16 x, qint16 y);
    bool mouse_pressed(qint16 x, qint16 y);
    bool mouse_release(qint16 x, qint16 y);
    void check_mouse_is_near_line(qint16 x, qint16 y);
    QJsonObject line_to_json(const LineTypesNS::Line & line);
    QString get_line_display_text(const LineTypesNS::Line & line);

    void fix(qint16 &x, qint16 &y);//同步鼠标坐标到图片翻转坐标

    static LineManager * _single_instace;
    std::map<QString,std::shared_ptr<std::list<std::shared_ptr<LineTypesNS::Line>>>> _all_lines;
    QString _current_picture_id;
    qint64 _current_patient_id = -1;
    std::shared_ptr<std::list<std::shared_ptr<LineTypesNS::Line>>> _current_picture_lines; //选中图片所有的线
    std::shared_ptr<std::list<std::shared_ptr<LineTypesNS::Line>>> _current_undo_lines; //用于撤销和反撤销功能
    QList<Operate> _undo_list;//记录操作顺序
    int _undo_index;//记录撤销位置

    std::shared_ptr<LineTypesNS::Line> _current_task_line; //当前操作的线
    int _current_clicked_index_pos = -1;//当点击到某条的点时点的索引，画线，和修改点时会频繁使用此参数
    LineTask _line_task = LineTask::NoLineTask; //当前线任务
    MouseTask _mouse_task = MouseTask::NoMouseTask; //当前鼠标任务
    LineTypesNS::Point _old_line_moving_pos; //旧的移动左边
    bool _releasing_for_end_move_line_or_change_point;
    bool _show_cross_mouse = false; //显示十字鼠标

    float _scale_factor = 1.0;
    bool _hor_mirror = false;
    bool _ver_mirror = false;
    int _rotate_factor = 0;
    qint16 _current_picture_width =20;
    qint16 _current_picture_height = 20;
    bool _need_fix = false;
    bool _show_rule = false;

    float _hor_per_pixel_mm; //选中图片的水平像素间距
    float _ver_per_pixel_mm;//选中图片的垂直像素间距
    LineTypesNS::Unit _line_unit = LineTypesNS::Unit::Mm; //当前所有图片的每个像素所选的长度单位
    qint64 _checked_line_id = -1; //选中的线id

    QString _current_line_color;//新增
    QString _current_angle_color;//新增
    QString _current_pen_color;//新增

    //当前画笔功能
    QPainterPath _cur_painter_path; //新增
    QPolygon _cur_polygon; //新增

    std::shared_ptr<LineTypesNS::Line> _current_task_pen; //当前操作的线 //新增

    bool _line_undo = false; //新增
    bool _line_redo = false; //新增
    
  //  QSettings settings;
};

#endif // PICTURELINEMANAGER_H
