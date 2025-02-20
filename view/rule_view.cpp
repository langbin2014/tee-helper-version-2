#include "rule_view.h"
#include<QPainter>
#include"c++/managers/line_manager.h"
RuleView::RuleView(QQuickItem *parent):QQuickPaintedItem(parent)
{
    setRenderTarget(QQuickPaintedItem::RenderTarget::FramebufferObject);
    setMipmap(false);
    connect(this,&RuleView::endChanged,[this]{
        this->update();
    });
    connect(this,&RuleView::beginChanged,[this]{
        this->update();
    });
    connect(this,&RuleView::is_horChanged,[this]{
        this->update();
    });
    connect(this,&RuleView::widthChanged,[this]{
        this->update();
    });
    connect(this,&RuleView::heightChanged,[this]{
        this->update();
    });
    connect(this,&RuleView::per_pixel_mmChanged,[this]{
        this->update();
    });
}

void RuleView::request_paint()
{
  this->update();
}

void RuleView::paint(QPainter *painter)
{

    if(width() < 10 || height() < 10 ||  _end < 10)
        return;
    int offset = 20;
    int len = (_is_hor ? width() : height())-2 * offset;
    int step = 4;
    int steps = len /step;

    QPoint start,end,line_step, child_line_offset ;

    if(_is_hor)
    {
        line_step = QPoint(step,0);
        start = QPoint(offset,height()-1);
        end = QPoint(offset+len,height() - 1);
        child_line_offset = QPoint(0,-4);
    }
    else
    {
        line_step = QPoint(0,-step);
        start = QPoint(1,height()-offset);
        end = QPoint(1,offset);
        child_line_offset = QPoint(4,0);
    }

    QPen pen;
    pen.setColor(QColor("yellow"));
    const float line_width =1.0;
    pen.setWidthF(line_width);
    painter->setPen(pen);//;
    QFont font("微软雅黑",8,QFont::Normal);
    painter->setFont(font);
    painter->drawLine(start,end);

    LineTypesNS::Unit unit = LineTypesNS::Unit(LineManager::get_instance()->getLine_unit());
    float result_unit = _per_pixel_mm;
    if(unit == LineTypesNS::Unit::Cm)
        result_unit = result_unit / 10.0f;
    else if(unit == LineTypesNS::Unit::In)
        result_unit = result_unit * 0.039370f;
    else if(unit == LineTypesNS::Unit::Px)
        result_unit = 1.0;
    float factor = 1.0f/(float(len)/float(step))*(_end - _begin)* result_unit;
    for(int i =0; i < steps;i ++)
    {
        QPoint current = start+(i*line_step);
        if(i % 10 ==0)
        {
            QPoint text_pos = current + (child_line_offset*6);
            painter->drawLine(current,current + (child_line_offset*4));
            QString str = QString::number(float(i) *factor,'f',1);
            int str_width = painter->fontMetrics().horizontalAdvance(str);
            if(_is_hor)
                text_pos = text_pos + QPoint(-str_width/2,0);
            else
                text_pos = text_pos +QPoint(0,+4);
            painter->drawText(text_pos,str);
        }
        else if(i % 5 ==0)
            painter->drawLine(current,current + (child_line_offset*2));
        else
            painter->drawLine(current,current + child_line_offset);
    }
}

float RuleView::begin() const
{
    return _begin;
}

void RuleView::setBegin(float begin)
{
    if(_begin != begin)
    {
        _begin = begin;
        emit beginChanged();
    }
}

float RuleView::end() const
{
    return _end;
}

void RuleView::setEnd(float end)
{
    if(_end != end)
    {
        _end = end;
        emit endChanged();
    }
}

bool RuleView::is_hor() const
{
    return _is_hor;
}

void RuleView::setIs_hor(bool is_hor)
{
    if(_is_hor != is_hor)
    {
        _is_hor = is_hor;
        emit is_horChanged();
    }
}



float RuleView::per_pixel_mm() const
{
    return _per_pixel_mm;
}

void RuleView::setPer_pixel_mm(float per_pixel_mm)
{
    if(_per_pixel_mm != per_pixel_mm)
    {
        _per_pixel_mm = per_pixel_mm;
        emit per_pixel_mmChanged();
    }
}
