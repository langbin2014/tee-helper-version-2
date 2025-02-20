#include "editing_image_provider.h"
#include"c++/log.h"
#include"c++/image_factor.h"
EditingImageProvider::EditingImageProvider():QQuickImageProvider(QQuickImageProvider::Image)
{

}

QImage EditingImageProvider::requestImage(const QString &info, QSize *size, const QSize &requestedSize)
{//id+origin_or_last+pts+#small?
    qDebug() <<"请求图片"<<info;
    QStringList list = info.split("/");
    QString id = list[0];
    bool small = info.endsWith("#small");
   // QString origin_or_last = list[1];

//    ImageState state ;
//    if(origin_or_last == "true")
//        state =  ImageState::Origin;
//    else if(origin_or_last == "false")
//        state = ImageState::Last;
//    else
//    {
//        Log::e(TAG,"代码逻辑错误，不是原图也不是最后的图片:"+origin_or_last);
//        QImage ret_404(":/resource/iamges/404.jpg");
//        if (size)
//            *size = ret_404.size();
//        return ret_404;
//    }

   std::shared_ptr<QImage> ret = ImageFactory::get_instance()->get_image(id);

    if(ret.get() != nullptr)
    {
        if(DEBUG)
            Log::d(TAG,"找到图片id:"+id);

        if(small == false)
        {
            if (size)
                *size = ret->size();
            return *ret;
        }
        else
        {
            QSize size2 = ret->size();
            size2 = size2/8.0;
            if (size)
                *size = size2;
            return ret->scaled(size2,Qt::KeepAspectRatio,Qt::SmoothTransformation);
        }
    }
    else
    {
        Log::e(TAG,"找不到图片:id="+id);
        QImage ret_404(640,320,QImage::Format::Format_Grayscale16);//(":/resource/iamges/404.jpg");
        ret_404.fill(QColor("black"));
        if (size)
            *size = ret_404.size();
        return ret_404;
    }
}
/*
void EditingImageProvider::addImage(const QString &id, EditingImageProvider::ImageData image)
{
    QMutexLocker l(&image_get_set_lock);
    pictures[id] = image;
}

bool EditingImageProvider::removeImage(const QString &id)
{
    QMutexLocker l(&image_get_set_lock);
    auto it = pictures.find(id);
    if(it != pictures.end())
    {
        pictures.erase(it);
        return true;
    }
    else
        return false;
}

void EditingImageProvider::clearImage()
{
    QMutexLocker l(&image_get_set_lock);
    pictures.clear();
}

QMutex *EditingImageProvider::get_iamge_read_write_lock()
{
    return &iamge_read_write_lock;
}

std::shared_ptr<QImage> EditingImageProvider::getImage(const QString &id, ImageState state)
{
    QMutexLocker l(&image_get_set_lock);
    std::shared_ptr<QImage> ret(0);
    auto it = pictures.find(id);
    if(it != pictures.end())
    {
        if(state == ImageState::Origin)
        {
            if(it->second.origin.get() == nullptr)
            {
                if(it->second.origin_path.isNull() == false)
                {
                    QMutexLocker l2(&iamge_read_write_lock);
                    QImage * temp = new QImage(it->second.origin_path);
                    it->second.origin = std::shared_ptr<QImage>(temp);

                }
            }
            ret = it->second.origin;
        }
        else if(state == ImageState::Last)
        {
            if(it->second.last.get() == nullptr)
            {
                if(it->second.last_path.isNull() == false)
                {
                    QMutexLocker l2(&iamge_read_write_lock);
                    QImage * temp = new QImage(it->second.last_path);
                    it->second.last = std::shared_ptr<QImage>(temp);
                }
            }
            ret = it->second.last;
        }
    }

    return ret;
}

bool EditingImageProvider::setImage(const QString &id, std::shared_ptr<QImage> new_image, ImageState state)
{
    QMutexLocker l(&image_get_set_lock);
    auto it = pictures.find(id);
    if(it != pictures.end())
    {
        if(state == ImageState::Last)
            it->second.last = new_image;
        else
            it->second.origin = new_image;
        return true;
    }
    return false;
}
*/
