#ifndef LINE_TYPE_H
#define LINE_TYPE_H
#include<QMutex>
#include<QString>
#include<vector>
namespace  LineTypesNS{
const float MAX_DIS = 8.0;
enum class Type{ //线类型
    Normal_Line, Angle_Line, Pen//普通线，角度线，画笔（不规则的线）
};

struct Point{ //点
    qint16 x;
    qint16 y;
};

struct Line{//线数据结构
    qint64 line_id;
    qint64 patient_id;
    QString image_id;
    Type line_type;
    // QByteArray line_data;
    std::vector<Point> line_data;
    QString line_color;
    QMutex line_data_read_write_lock;
};

enum  Unit{Mm,Cm,In,Px};

};
#endif // LINE_TYPE_H
