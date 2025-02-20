#ifndef RULEVIEW_H
#define RULEVIEW_H

#include<QQuickPaintedItem>
#include"c++/line_type.h"
class RuleView :public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(float begin READ begin   WRITE setBegin NOTIFY beginChanged)
    Q_PROPERTY(float end READ end   WRITE setEnd NOTIFY endChanged)
    Q_PROPERTY(bool is_hor READ is_hor   WRITE setIs_hor NOTIFY is_horChanged)
    Q_PROPERTY(float per_pixel_mm  READ per_pixel_mm   WRITE setPer_pixel_mm NOTIFY per_pixel_mmChanged)

public:

    RuleView(QQuickItem * parent = nullptr);
    Q_INVOKABLE void request_paint();
    virtual void  paint(QPainter * painter) override;
    float begin() const;
    void setBegin(float begin);

    float end() const;
    void setEnd(float end);

    bool is_hor() const;
    void setIs_hor(bool is_hor);

    int line_unit() const;
    void setLine_unit(const int &line_unit);

    float per_pixel_mm() const;
    void setPer_pixel_mm(float per_pixel_mm);

signals:
    void beginChanged();
    void endChanged();
    void is_horChanged();
    void per_pixel_mmChanged();

private:
    float _begin;
    float _end;

    bool _is_hor;
    float _per_pixel_mm;

};

#endif // RULEVIEW_H
