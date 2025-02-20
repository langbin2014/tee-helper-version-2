#ifndef LINES_TABLE_H
#define LINES_TABLE_H

#include <QObject>
#include <QMetaEnum>
#include "database.h"
#include "c++/line_type.h"

class LineTable :public QObject,public DataBase
{
    const bool DEBUG = true;
    const QString TAG = "线表";
    Q_OBJECT
public:
    enum class Field{ //添加字段都要在 query函数里面添加对应的字段转换
        Image_ID_TEXT = 0, //图片id,是时间戳,固定
        Patient_ID_INTEGER, //病人id //固定
        Line_ID_INTEGER, //线id //固定
        Line_Type_integer, //线类型 //固定
        Line_Data_BLOB, //float 二进制 //可变
        Line_Color_TEXT,//线颜色 16进制   //可变
        End_DataBase_Field
    };
    Q_ENUM(Field);

    static LineTable * getInstance();//
    void create_and_init_a_picture(QJsonObject &picture_info,const QString & path,qint64 pts,qint64 patient_id, const QString & source_type)const;
    inline  QString get_field_name(Field field);//
    inline int get_field_value(const QString & field_str);//
    bool add(const LineTypesNS::Line & line );//qint64 line_id, quint64 patient_id, const QString & image_id,const QByteArray & data,const QString & color,LineType line_type);
    void update_color(qint64 id, const QString &color);
    void update_line_data(qint64 id,const std::vector<LineTypesNS::Point> & line_data);
    void remove(qint64 line_id);//
    void remove_this_picture_all_lines(const QString & picture_id);
    void remove_this_patient_all_lines(qint64 patient_id);
    void query_this_patient_lines(qint64 patient_id, std::map<QString, std::shared_ptr<std::list<std::shared_ptr<LineTypesNS::Line> >>> &out);
protected:
    virtual void on_creating_table() override;
    virtual QString table_name()override;

private:
    LineTable(QObject * parent = nullptr);
    QMetaEnum meta;// = QMetaEnum::fromType<images_table::Field>();
    static LineTable *  _single_instance;
};

#endif // LINES_TABLE_H
