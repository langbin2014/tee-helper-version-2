#ifndef EDITINGIMAGEPROVIDER_H
#define EDITINGIMAGEPROVIDER_H
#include<QMutex>
#include<QQuickImageProvider>
#include"c++/image_types.h"
class EditingImageProvider :public QQuickImageProvider
{
    const QString TAG = "图片提供类";
    const bool DEBUG = true;
public:

//    struct ImageData{
//      std::shared_ptr<QImage>  origin = nullptr;
//      std::shared_ptr<QImage>  last = nullptr;
//      QString origin_path;
//      QString last_path;
//    };
    EditingImageProvider();
    virtual QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
    /*
    void addImage(const QString & id,ImageData image);
    bool removeImage(const QString & id);
    void clearImage();
    QMutex * get_iamge_read_write_lock();
    std::shared_ptr<QImage> getImage(const QString & id, ImageState state);
    bool setImage(const QString & id,std::shared_ptr<QImage> new_image, ImageState  state);
    */
private:

//    QMutex image_get_set_lock;
//    QMutex iamge_read_write_lock; //硬盘读写锁
//    std::map<QString,ImageData> pictures;


};

#endif // EDITINGIMAGEPROVIDER_H
