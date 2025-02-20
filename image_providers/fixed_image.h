
#ifndef FIXED_IMAGE_H
#define FIXED_IMAGE_H



#include <QQuickImageProvider>
#include <QMutex>
class Fixed_Image:public QQuickImageProvider
{
public:
    Fixed_Image();
    virtual QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
    void set_dark_image_data(const QByteArray & data);
    void set_light_image_data(const QByteArray & data);
private:
    QImage dark,light;
    int index =1;
};

#endif // FIXED_IMAGE_H
