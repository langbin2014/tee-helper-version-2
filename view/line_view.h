#ifndef LINEVIEW_H
#define LINEVIEW_H

#include<QQuickPaintedItem>
#include "c++/line_type.h"
class LineView:public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString   picture_id        READ getPicture_id    WRITE setPicture_id    NOTIFY picture_idChanged);
    Q_PROPERTY(bool hor_mirror        READ getHor_mirror    WRITE setHor_mirror NOTIFY hor_mirrorChanged)
    Q_PROPERTY(bool ver_mirror        READ getVer_mirror    WRITE setVer_mirror NOTIFY ver_mirrorChanged)
    Q_PROPERTY(float scale_factor        READ getScale_factor    WRITE setScale_factor NOTIFY scale_factorChanged)
    Q_PROPERTY(int rotate_factor        READ getRotate_factor   WRITE setRotate_factor NOTIFY rotate_factorChanged)
    Q_PROPERTY(float hor_per_pixel_mm  READ getHor_per_pixel_mm   WRITE setHor_per_pixel_mm NOTIFY hor_per_pixel_mmChanged)
    Q_PROPERTY(float ver_per_pixel_mm  READ getVer_per_pixel_mm    WRITE setVer_per_pixel_mm  NOTIFY ver_per_pixel_mmChanged)

public:
    Q_INVOKABLE void request_paint();
    LineView(QQuickItem * parent = nullptr);
    virtual void  paint(QPainter * painter) override;

    QString getPicture_id() const;
    void setPicture_id(const QString &picture_id);

    bool getHor_mirror() const;
    void setHor_mirror(bool hor_mirror);

    bool getVer_mirror() const;
    void setVer_mirror(bool ver_mirror);

    float getScale_factor() const;
    void setScale_factor(float scale_factor);

    int getRotate_factor() const;
    void setRotate_factor(int rotate_factor);

    float getHor_per_pixel_mm() const;
    void setHor_per_pixel_mm(float hor_per_pixel_mm);

    float getVer_per_pixel_mm() const;
    void setVer_per_pixel_mm(float ver_per_pixel_mm);

    Q_INVOKABLE bool get_fix(){
        return _need_fix;
    }
signals:
    void picture_idChanged();
    void hor_mirrorChanged();
    void ver_mirrorChanged();
    void scale_factorChanged();
    void rotate_factorChanged();
    void hor_per_pixel_mmChanged();
    void ver_per_pixel_mmChanged();
private:
    void draw_circle(const std::shared_ptr<LineTypesNS::Line> & line, QPainter *painter);
    void draw_text(const std::shared_ptr<LineTypesNS::Line> &line, QPainter *painter, LineTypesNS::Unit unit);
    void fix(qint16 &x, qint16 &y);
    const  QString TAG = "画板类";
    const  bool DEBUG = false;
    QString _picture_id;
    float _scale_factor = 1.0;
    bool _hor_mirror = false;
    bool _ver_mirror = false;
    int _rotate_factor = 0;
    bool _need_fix = false;
    float _hor_per_pixel_mm; //选中图片的水平像素间距
    float _ver_per_pixel_mm;//选中图片的垂直像素间距
};

#endif // LINEVIEW_H





