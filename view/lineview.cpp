#include "line_view.h"
#include "c++/managers/line_manager.h"
#include<QPen>
#include<QPainter>
#include<cmath>
#include"c++/log.h"
void LineView::request_paint()
{
    update();
}

LineView::LineView(QQuickItem *parent):QQuickPaintedItem(parent)
{
    setRenderTarget(QQuickPaintedItem::RenderTarget::Image);
    setMipmap(false);

    connect(this,&LineView::hor_mirrorChanged,[this]{
        this->update();
    });
    connect(this,&LineView::ver_mirrorChanged,[this]{
        this->update();
    });
    connect(this,&LineView::scale_factorChanged,[this]{
        this->update();
    });
    connect(this,&LineView::rotate_factorChanged,[this]{
        this->update();
    });
}

void LineView::paint(QPainter *painter)
{
    LineManager * instance = LineManager::get_instance();

    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setColor(instance->getCurrent_pen_color());//获取当前画笔线的颜色
    const float line_width = 3.0;
    pen.setWidthF(line_width / _scale_factor * 2);//线宽 除以 图像缩放比例
    painter->setPen(pen);//;
    painter->setClipRect(0,0,width(),height());//绘制区域
    QPainterPath painter_path;
    instance->getPainterPath(painter_path);
    painter->drawPath(painter_path);

    LineTypesNS::Unit line_unit =  LineTypesNS::Unit(instance->getLine_unit());
    auto lines = instance->get_lines(_picture_id);


    if(lines == nullptr)
    {
        if(DEBUG)
            Log::d(TAG,"获取不到线，取消渲染");
        return;
    }

    if(lines->size() == 0)
    {
        if(DEBUG)
            Log::d(TAG,"线长度为0，取消渲染");
        return;
    }
    auto current_task_line = instance->get_current_task_line();

    painter->setRenderHint(QPainter::Antialiasing, true);
    float p_facotr = 16.0;
    QFont font("微软雅黑",p_facotr,QFont::Normal);
    font.setPointSizeF(p_facotr/_scale_factor);
    painter->setFont(font);
    for(auto it = lines->begin(); it != lines->end();it ++)
    {
        QMutexLocker l(&((*it)->line_data_read_write_lock));
        std::vector<LineTypesNS::Point> & line_data = (*it)->line_data;
        if ((*it)->line_type == LineTypesNS::Type::Pen) {
            QPolygon polygon;
            QPainterPath path;
            pen.setWidthF(line_width / _scale_factor * 2);//线宽 除以 图像缩放比例
            pen.setColor(QColor((*it)->line_color));
            painter->setPen(pen);
            for(int i = 0;i < (int)line_data.size(); i++)
            {
                polygon.append(QPoint(line_data[i].x, line_data[i].y));
                path = QPainterPath();
                path.addPolygon(polygon);
            }
            painter->drawPath(path);
            continue;
        }

        if(line_data.size() < 2 ) //2点
        {
            if(DEBUG)
                Log::d(TAG,"线点数过小，跳过渲染");
            continue;
        }
        else
        {
            if(DEBUG)
                Log::d(TAG,"渲染");
        }


        QString color = (*it)->line_color;
        LineTypesNS::Type  line_tye = (*it)->line_type;
        QPen pen;
        if(line_tye == LineTypesNS::Type::Angle_Line) //角度画虚线
            pen.setStyle(Qt::DashLine);
        pen.setColor(QColor(color));
//        const float line_width =1.0;
        pen.setWidthF(line_width/_scale_factor);
        painter->setPen(pen);//;
//        painter->setClipRect(0,0,width(),height());
        for(int i =0 ;i + 1 < (int)line_data.size();i++)
        {
            qint16 x1 = line_data[i].x;
            qint16 y1 = line_data[i].y;
            qint16 x2 = line_data[i+1].x;
            qint16 y2 = line_data[i+1].y;
            fix(x1,y1);
            fix(x2,y2);
            if(DEBUG)
                qDebug() << x1 << "," << y1 << "," << x2 << "," << y2;
            painter->drawLine(x1,y1,x2,y2);
        }

        if(current_task_line != nullptr && *it == current_task_line) //这个线就是正在编辑的线
            draw_circle(*it,painter); //画圆圈
        draw_text(*it,painter,line_unit);
    }
}

QString LineView::getPicture_id() const
{
    return _picture_id;
}

void LineView::setPicture_id(const QString &picture_id)
{
    if(_picture_id != picture_id)
    {
        _picture_id = picture_id;
        emit picture_idChanged();
    }
}

void LineView::draw_circle(const std::shared_ptr<LineTypesNS::Line> & line,QPainter * painter)
{
    std::vector<LineTypesNS::Point> & line_data = line->line_data;
    for(int i =0 ;i < line_data.size();i++)
    {
        qint16 x = line_data[i].x;
        qint16 y = line_data[i].y;
        fix(x,y);
        painter->drawEllipse(QPointF(x,y),LineTypesNS::MAX_DIS/_scale_factor,LineTypesNS::MAX_DIS/_scale_factor);
    }
}

void LineView::draw_text(const std::shared_ptr<LineTypesNS::Line> & line, QPainter *painter,LineTypesNS::Unit unit)
{
    auto & points = line->line_data;
    if(line->line_type == LineTypesNS::Type::Normal_Line)
    {
        float len =0;
        for(auto it = points.begin();it != points.end();it++)
        {
            auto next = it+1;
            if(next == points.end())
                break;

            qint16 pixel_x1 = it->x;
            qint16 pixel_y1 = it->y;
            qint16 pixel_x2 = next->x;
            qint16 pixel_y2 = next->y;

            if(unit == LineTypesNS::Unit::Px)
            {
                float two_point_dis_px =  std::sqrt(std::pow(pixel_x1 -pixel_x2,2) + std::pow(pixel_y1-pixel_y2,2));
                len += int(two_point_dis_px);
            }
            else {
                float x1 = pixel_x1 * _hor_per_pixel_mm;
                float x2 = pixel_x2 * _hor_per_pixel_mm;
                float y1 = pixel_y1 * _ver_per_pixel_mm;
                float y2 = pixel_y2 * _ver_per_pixel_mm;
                float two_point_dis_mm =  std::sqrt(std::pow(x1 -x2,2) + std::pow(y1-y2,2));
                float result = two_point_dis_mm;
                if(unit == LineTypesNS::Unit::Cm)
                    result = result / 10.0f;
                else if(unit == LineTypesNS::Unit::In)
                    result = result * 0.039370f;
                len += result;
            }
        }
        QString unit_str;
        int little_len = 0;
        if(unit == LineTypesNS::Unit::Cm)
        {
            unit_str= tr("cm");
            little_len = 2;
        }
        else if(unit == LineTypesNS::Unit::In)
        {
            unit_str= tr("in");
            little_len = 3;
        }
        else if(unit == LineTypesNS::Unit::Mm)
        {
            unit_str = tr("mm");
            little_len = 1;
        }
        else if(unit == LineTypesNS::Unit::Px)
        {
            unit_str = tr("px");
            little_len = 0;
        }
        else
        {
            unit_str = tr("error");
            little_len = 4;
        }
        QString ret = QString::number(len,'f',little_len)+QString("(%1)").arg(unit_str) ;
        auto & last_point = points.at(points.size()-1);
        qint16 x = last_point.x;
        qint16 y = last_point.y;
        fix(x,y);
        float offset = 20.0 / _scale_factor;
        if(_rotate_factor == 0 || _rotate_factor == 360 || _rotate_factor == -360)
            painter->drawText(x+offset,y,ret);
        else if(_rotate_factor == 180 || _rotate_factor == -180)
        {
            painter->save();
            painter->translate(x-offset,y);
            painter->rotate(360 - _rotate_factor);
            painter->drawText(0,0,ret);
            painter->restore();
        }
        else
        {
            painter->save();
            painter->translate(x+offset,y);
            painter->rotate(360 - _rotate_factor);
            painter->drawText(0,0,ret);
            painter->restore();
        }
    }
    else if(line->line_type == LineTypesNS::Type::Angle_Line)
    {
        QPen pen;
        pen.setColor(QColor(line->line_color));
        const float line_width =3.0;
        pen.setWidthF(line_width/_scale_factor);
        painter->setPen(pen);//;

        float angle =0;
        const float M_PI =3.14159265;
        int size = points.size();
        for(int i = 1;i < size-1;i++)
        {
            auto point_1 = points.at(i-1);
            fix(point_1.x,point_1.y);
            auto point_2 = points.at(i);
            fix(point_2.x,point_2.y);
            auto point_3 = points.at(i+1);
            fix(point_3.x,point_3.y);

            //  painter->drawText(point_2.x,point_2.y- 20,QString("(%1,%2)").arg(point_2.x).arg(point_2.y));

            double theta_2_1 = atan2( point_2.y - point_1.y,point_1.x - point_2.x);
            double theta_2_3 = atan2( point_2.y - point_3.y,point_3.x - point_2.x);

            double theta = theta_2_1 - theta_2_3;

            if (theta > M_PI)
                theta -= 2 * M_PI;
            if (theta < -M_PI)
                theta += 2 * M_PI;
            //            if(i == size -2 )
            //                qDebug() << point_1.x<<" " << point_1.y<<" " << point_2.x<<" " << point_2.y << point_1.x - point_2.x << point_1.y - point_2.y;
            //            if(i == size-2)
            //                qDebug() << "原起始角" << theta_2_1*180.0 / M_PI;

            if(theta_2_1 < 0)
                theta_2_1 = 2.0*M_PI + theta_2_1;
            if(theta_2_3 < 0)
                theta_2_3 = 2.0*M_PI + theta_2_3;

            double begin = theta_2_1*180.0 / M_PI;

            //            if(i == size-2)
            //                qDebug() << "改变后的起始角" << theta_2_1*180.0 / M_PI;

            bool clockwise = false;
            if(theta_2_1 < theta_2_3)
            {
                if(theta_2_3 - theta_2_1 > M_PI)
                    clockwise = true;
                else
                    clockwise =false;
            }
            else{
                if(theta_2_1 - theta_2_3 > M_PI)
                    clockwise = false;
                else
                    clockwise = true;
            }
            //clockwise = std::abs(theta_2_1 -theta_2_3) > M_PI; //顺时针
            float facotor = clockwise ?  -1 : 1;
            angle = abs(theta * 180.0 / M_PI);
            float r = 15/ _scale_factor;;
            QRectF rect(point_2.x-r/2,point_2.y-r/2,r,r);
            //QString ret = QString::number(angle,'f',2)+QString("°") ;
            painter->drawArc(rect,begin*16,angle*facotor*16);

            float mid_angle = begin + (angle*facotor)/2.0;
            if(mid_angle < -360)
                mid_angle += 360;
            if(mid_angle > 360)
                mid_angle -= 360;
            float mid_radius = mid_angle*M_PI/180.0;
            int x_offset = std::cos(mid_radius) * r * 1.2;
            int y_offset = std::sin(mid_radius) *r * (-1.2);

            QString ret = QString::number(angle,'f',1)+QString("°");
            int str_width = painter->fontMetrics().horizontalAdvance(ret);
            if(_rotate_factor == 0 || _rotate_factor == 360 || _rotate_factor == -360)
            {
                if(x_offset < 0 && !_ver_mirror && !_hor_mirror)
                {
                    x_offset =x_offset -str_width;
                }
                painter->drawText(point_2.x+x_offset,point_2.y+y_offset,ret);
            }
            else if(_rotate_factor == 180 || _rotate_factor == -180)
            {
                painter->save();
                painter->translate(point_2.x+x_offset,point_2.y+y_offset);
                painter->rotate(360 - _rotate_factor);
                painter->drawText(0,0,ret);
                painter->restore();
            }
            else
            {
                painter->save();
                painter->translate(point_2.x+x_offset,point_2.y+y_offset);
                painter->rotate(360 - _rotate_factor);
                painter->drawText(0,0,ret);
                painter->restore();
            }
        }
    }
}

void LineView::fix(qint16 & x, qint16 & y)
{
    if(_need_fix)
    {//可能要改成4个旋转角度+和2种状态的8种情况进行计算
        if(_hor_mirror)
        {
            y = height()-y;
        }
        if(_ver_mirror)
        {
            x = width()-x;
        }
    }
    else //不需要修复
    {
        if(_hor_mirror)
        {
            x = width()-x;
        }
        if(_ver_mirror)
        {
            y = height()-y;
        }
    }

}


bool LineView::getHor_mirror() const
{
    return _hor_mirror;
}

void LineView::setHor_mirror(bool hor_mirror)
{
    if(_hor_mirror != hor_mirror)
    {
        _hor_mirror = hor_mirror;
        emit hor_mirrorChanged();
    }
}

bool LineView::getVer_mirror() const
{
    return _ver_mirror;
}

void LineView::setVer_mirror(bool ver_mirror)
{
    if(_ver_mirror != ver_mirror)
    {
        _ver_mirror = ver_mirror;
        emit ver_mirrorChanged();
    }
}


float LineView::getScale_factor() const
{
    return _scale_factor;
}

void LineView::setScale_factor(float scale_factor)
{
    if(_scale_factor != scale_factor)
    {
        _scale_factor = scale_factor;
        emit scale_factorChanged();
    }
}

int LineView::getRotate_factor() const
{
    return _rotate_factor;
}

void LineView::setRotate_factor(int rotate_factor)
{

    if(_rotate_factor != rotate_factor)
    {
        _rotate_factor = rotate_factor;
        int ro = _rotate_factor < 0 ?  -_rotate_factor : _rotate_factor;
        _need_fix = ro  % 180 != 0; //需要修复
        emit rotate_factorChanged();
    }
    else
    {
        int ro = _rotate_factor < 0 ?  -_rotate_factor : _rotate_factor;
        _need_fix = ro  % 180 != 0; //需要修复
    }
}

float LineView::getHor_per_pixel_mm() const
{
    return _hor_per_pixel_mm;
}

void LineView::setHor_per_pixel_mm(float hor_per_pixel_mm)
{
    if(_hor_per_pixel_mm != hor_per_pixel_mm)
    {
        _hor_per_pixel_mm = hor_per_pixel_mm;
        emit hor_per_pixel_mmChanged();
    }
}

float LineView::getVer_per_pixel_mm() const
{
    return _ver_per_pixel_mm;
}

void LineView::setVer_per_pixel_mm(float ver_per_pixel_mm)
{
    if(_ver_per_pixel_mm != ver_per_pixel_mm)
    {
        _ver_per_pixel_mm = ver_per_pixel_mm;
        emit ver_per_pixel_mmChanged();
    }
}
