#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H
#include<QImage>
#include<QMutex>
#include"image_types.h"
#include<memory>
class ImageFactory
{
    const bool DEBUG = true;
    const QString TAG = "图片工厂类";
     ImageFactory();
public:
     struct ImageData{
       std::shared_ptr<QImage>  origin = nullptr;
       std::shared_ptr<QImage>  last = nullptr;
       QString origin_path;
       QString last_path;
       ImageState state = ImageState::Origin;
       int Rui_Hua_integer = 0;
     };

    static QImage * sharpen(float factor,const QImage & input);
    bool sharpen(const QString & id, int factor);
    static QImage * use_default_value_create_an_final_image(const QImage & input);
    static ImageFactory * get_instance();
    static void mouse_blur(const QImage & input, QImage & output);
   bool synchronized_image(const QString & picture_id,const QJsonObject & some_params);
    bool apply_filter(const QString & id, ImageFilter filter);

    void add_image(const QString & id,ImageData image);
    bool remove_image(const QString & id);
    void clear_image();
   // QMutex * get_iamge_read_write_lock();
    std::shared_ptr<QImage> get_image(const QString & id, ImageState state);
    std::shared_ptr<QImage> get_image(const QString & id);
    bool set_image(const QString & id,std::shared_ptr<QImage> new_image, ImageState  state);
    bool set_image_state(const QString & id, ImageState state);
    bool set_image_rui_hua_value(const QString & id, int value);
    bool get_final_image(const QString &id,QImage & out,const QJsonObject & info);
    bool get_final_image_fake_color(const QString &id,QImage & out,const QJsonObject & info);
     bool get_final_image_gray(const QString &id,QImage & out,const QJsonObject & info);
    void float_(const QImage & in, QImage & out,float factor);
    static QImage * filter(ImageFilter filter,const QImage & input);
private:

    static ImageFactory * _self;
    QMutex image_get_set_lock;
 //   QMutex iamge_read_write_lock; //硬盘读写锁
    std::map<QString,ImageData> pictures;

};

#endif // IMAGEPROCESSOR_H
