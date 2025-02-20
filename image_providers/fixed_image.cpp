
#include "fixed_image.h"
#include<QDebug>
#include"c++/managers/patient_manager.h"
static QImage  no_scale_2(int w,int h,const QByteArray & buf)
{

    quint16 * buf_ct_value = new quint16[w * h];
   const QByteArray & data  = buf;
  const  quint16 * ptr = (const quint16*)data.data();
    for(int i =0; i < w*h;i++)
    {
        quint16 ct_value = ptr[i];
        buf_ct_value[i] = 16*ct_value;
    }

   return QImage((unsigned char *)buf_ct_value,w,h,QImage::Format::Format_Grayscale16);
}
Fixed_Image::Fixed_Image():QQuickImageProvider(QQuickImageProvider::Image)
{
   light= dark = QImage(":/images/baby_icon.png");
}

QImage Fixed_Image::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
   qDebug() << "请求图" << id;
    if(id.contains("dark"))
    {
        *size = dark.size();
        return  dark;
    }
    else
    {
        *size = light.size();
        return light;
    }
}

void Fixed_Image::set_dark_image_data(const QByteArray &data)
{
    auto pic_mg =PatientManager::get_instance()->get_picture_manager();
    if(!data.isEmpty())
    {
    dark = no_scale_2(1660,2280,data);
    pic_mg->set_dark_image_url("image://fixed_images/dark"+QString::number(index++));
    }
    else
    {

    dark = QImage();
    pic_mg->set_dark_image_url("image://fixed_images/dark"+QString::number(index++));

    }
}

void Fixed_Image::set_light_image_data(const QByteArray &data)
{
    auto pic_mg =PatientManager::get_instance()->get_picture_manager();
    if(!data.isEmpty())
    {
    light = no_scale_2(1660,2280,data);
    pic_mg->set_light_image_url("image://fixed_images/light"+QString::number(index++));
    }
    else
    {
    light = QImage();
    pic_mg->set_light_image_url("image://fixed_images/light"+QString::number(index++));
    }
}

