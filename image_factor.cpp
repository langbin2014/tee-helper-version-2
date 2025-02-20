#include<QDebug>
#include "image_factor.h"
#include"core/image_core.h"
#include"c++/table/images_table.h"
#include"log.h"
#include<QFile>
#include"configure.h"
const float M_PI = 3.1415926;

#define SWAP(a, b, t)  do { t = a; a = b; b = t; } while(0)
#define CLIP_RANGE(value, min, max)  ( (value) > (max) ? (max) : (((value) < (min)) ? (min) : (value)) )
#define COLOR_RANGE(value)  CLIP_RANGE(value, 0, 255)
int adjustBrightnessContrast8(cv::InputArray src, cv::OutputArray dst, int brightness, int contrast)
{
    cv::Mat input = src.getMat();
    if( input.empty() ) {
        return -1;
    }

    dst.create(src.size(), src.type());
    cv::Mat output = dst.getMat();

    brightness = CLIP_RANGE(brightness, -255, 255);
    contrast = CLIP_RANGE(contrast, -255, 255);

    /**
    Algorithm of Brightness Contrast transformation
    The formula is:
        y = [x - 127.5 * (1 - B)] * k + 127.5 * (1 + B);
        x is the input pixel value
        y is the output pixel value
        B is brightness, value range is [-1,1]
        k is used to adjust contrast
            k = tan( (45 + 44 * c) / 180 * PI );
            c is contrast, value range is [-1,1]
    */

    double B = double (brightness) / 255.;
    double c = double (contrast) / 255. ;
    double k = tan((45 + 44 * c) / 180 * M_PI);

    cv::Mat lookupTable(1, 256, CV_8U);
    uchar *p = lookupTable.data;
    for (int i = 0; i < 256; i++)
        p[i] = COLOR_RANGE( (i - 127.5 * (1 - B)) * k + 127.5 * (1 + B) );

    LUT(input, lookupTable, output);

    return 0;
}
ImageFactory * ImageFactory::_self = nullptr;
ImageFactory::ImageFactory()
{

}


QImage *ImageFactory::filter(ImageFilter filter, const QImage &input)
{

    if(filter == ImageFilter::NoFilter)
        return  new QImage(input);
    else if(filter == ImageFilter::MouseUSM)
    {

        QImage * out = new QImage;
        Core::ImageTool::sharpen(5.5,input,*out);
        return out;
    }
    else if(filter == ImageFilter::MouseHD)
    {
        QImage * out = new QImage;
        Core::ImageTool::mouse_hd(input,*out);
        return out;
    }
    else if(filter == ImageFilter::MouseBlur)
    {
        QImage * out = new QImage;
        mouse_blur(input, *out);
        return out;
    }
    else if(filter == ImageFilter::ClearVerLines)
    {
        QImage * out = new QImage;
        Core::ImageTool::clear_ver_lines(input,*out,7);
        return out;
    }
    else if(filter == ImageFilter::ImageLevelUp)
    {
        QImage * out = new QImage;
        Core::ImageTool::image_level_up(input,*out,7);
        return out;
    }
    else
    { //未完成
        Log::e("图片工厂类",QString("使用了不存在的滤镜:%1,已被忽略").arg(int(filter)));
        return  new QImage(input);
    }
}


QImage *ImageFactory::sharpen(float factor, const QImage &input)
{
    if(factor < 0.3)
    {
        return new QImage(input);
    }
    else
    {
        QImage * out = new QImage;
        Core::ImageTool::sharpen(factor,input,*out);
        return out;
    }
}

bool ImageFactory::sharpen(const QString &id, int factor)
{
    factor += 0.5;
    std::shared_ptr<QImage> ptr = get_image(id,ImageState::Last);
    if(ptr.get() == nullptr || ptr->isNull())
    {
        Log::e(TAG,"获取不到图片，已经取消锐化:"+id);
        return false;
    }
    else
    {
        QImage * temp =  sharpen(factor,*ptr);
        set_image(id,std::shared_ptr<QImage>(temp),ImageState::Last);
        return true;
    }
}

QImage *ImageFactory::use_default_value_create_an_final_image(const QImage &input)
{
    QImage * temp = ImageFactory::filter(Configure::filter,input);
    return temp;
}

ImageFactory *ImageFactory::get_instance()
{
    if(_self == nullptr)
        _self = new ImageFactory;
    return _self;
}

void ImageFactory::mouse_blur(const QImage &input, QImage &output)
{
    Core::ImageTool::mouse_blur(input,output);
}

bool ImageFactory::synchronized_image(const QString &picture_id, const QJsonObject &some_params)
{

    bool ok;
    {
        QString filter_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Filter_Index_integer);
        int filter = some_params[filter_key].toInt();
        ok = apply_filter(picture_id,ImageFilter(filter));
        if(!ok)
        {
            Log::e(TAG,"应用过滤器失败"+picture_id);
            return false;
        }
    }
    {
        QString rui_hua_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Rui_Hua_integer);
        int rui_hua = some_params[rui_hua_key].toInt();
        ok = set_image_rui_hua_value(picture_id,rui_hua);
        if(!ok)
        {
            Log::e(TAG,"设置锐化值失败"+picture_id);
            return false;
        }
    }
    {
        QString state_key = ImagesTable::getInstance()->get_field_name(ImagesTable::Field::Image_State_Integer);
        int state = some_params[state_key].toInt();
        ok = set_image_state(picture_id,ImageState(state));
        if(!ok)
        {
            Log::e(TAG,"设置图片状态失败:"+picture_id);
            return false;
        }
    }
    return true;
}

bool ImageFactory::apply_filter(const QString &id, ImageFilter filter)
{
    std::shared_ptr<QImage> ptr = get_image(id,ImageState::Origin);
    if(ptr.get() == nullptr || ptr->isNull())
    {
        Log::e(TAG,"获取不到图片，已经取消滤镜:"+id);
        return false;
    }
    else
    {
        QImage * temp = ImageFactory::filter(ImageFilter(filter),*ptr);
        //            *temp  = temp->convertToFormat(QImage::Format::Format_Grayscale8);
        //        cv::Mat src = cv::Mat(temp->height(),temp->width(),CV_8UC1,temp->bits(),temp->bytesPerLine()),dst;
        //        int b = (1.0 - 1.0)*255.0;
        //        int c = (1.0 - 1.0)*255.0;
        //        adjustBrightnessContrast8(src,dst,b,c);
        //        *temp = QImage(dst.data, dst.cols,dst.rows,sizeof(quint8)*dst.cols,QImage::Format::Format_Grayscale8).convertToFormat(QImage::Format::Format_Grayscale16);
        set_image(id,std::shared_ptr<QImage>(temp),ImageState::Last);
        return true;
    }
}

void ImageFactory::add_image(const QString &id, ImageFactory::ImageData image)
{
    QMutexLocker l(&image_get_set_lock);
    pictures[id] = image;
}

bool ImageFactory::remove_image(const QString &id)
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

void ImageFactory::clear_image()
{
    QMutexLocker l(&image_get_set_lock);
    pictures.clear();
}

//QMutex *ImageFactor::get_iamge_read_write_lock()
//{
//    return &iamge_read_write_lock;
//}

std::shared_ptr<QImage> ImageFactory::get_image(const QString &id, ImageState state)
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
                    // QMutexLocker l2(&iamge_read_write_lock);
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
                    //  QMutexLocker l2(&iamge_read_write_lock);
                    QImage * temp = new QImage(it->second.last_path);
                    it->second.last = std::shared_ptr<QImage>(temp);
                }
            }
            ret = it->second.last;
        }
    }

    return ret;
}

std::shared_ptr<QImage> ImageFactory::get_image(const QString &id)
{
    QMutexLocker l(&image_get_set_lock);
    std::shared_ptr<QImage> ret(0);
    auto it = pictures.find(id);
    if(it != pictures.end())
    {
        if(it->second.state == ImageState::Origin)
        {
            //            if(it->second.origin == nullptr)
            //            {
            //                if(it->second.origin_path.isNull() == false)
            //                {
            //                    QImage * temp = new QImage(it->second.origin_path);
            //                    it->second.origin = std::shared_ptr<QImage>(temp);
            //                }
            //            }
            ret = std::make_shared<QImage>(it->second.origin_path);  //it->second.origin;
        }
        else if(it->second.state == ImageState::Last)
        {
            if(it->second.Rui_Hua_integer > 0)
            {
                QImage pic_temp(it->second.last_path);
                if(!pic_temp.isNull())
                {
                    QImage * temp =  sharpen(it->second.Rui_Hua_integer +0.5,pic_temp);
                    ret = std::shared_ptr<QImage>(temp);
                }
            }
            else
            {
                ret = std::make_shared<QImage>(it->second.last_path);
            }
        }
    }

    return ret;
}

//std::shared_ptr<QImage> ImageFactory::get_image(const QString &id)
//{
//    QMutexLocker l(&image_get_set_lock);
//    std::shared_ptr<QImage> ret(0);
//    auto it = pictures.find(id);
//    if(it != pictures.end())
//    {
//        if(it->second.state == ImageState::Origin)
//        {
//            if(it->second.origin == nullptr)
//            {
//                if(it->second.origin_path.isNull() == false)
//                {
//                    QImage * temp = new QImage(it->second.origin_path);
//                    it->second.origin = std::shared_ptr<QImage>(temp);
//                }
//            }
//            ret = it->second.origin;
//        }
//        else if(it->second.state == ImageState::Last)
//        {
//            if(it->second.last == nullptr)
//            {
//                if(it->second.last_path.isNull() == false)
//                {
//                    QImage * temp = new QImage(it->second.last_path);
//                    it->second.last = std::shared_ptr<QImage>(temp);
//                }
//            }
//            if(it->second.last != nullptr)
//            {
//                if(it->second.Rui_Hua_integer > 0)
//                {
//                    std::shared_ptr<QImage> ptr = it->second.last;
//                    if(ptr == nullptr || ptr->isNull())
//                    {
//                        Log::e(TAG,"获取不到图片，已经取消锐化:"+id);
//                    }
//                    else
//                    {
//                        QImage * temp =  sharpen(it->second.Rui_Hua_integer +0.5,*ptr);
//                        ret = std::shared_ptr<QImage>(temp);
//                    }
//                }
//                else
//                    ret  = it->second.last;
//            }
//        }
//    }

//    return ret;
//}

bool ImageFactory::set_image(const QString &id, std::shared_ptr<QImage> new_image, ImageState state)
{
    QMutexLocker l(&image_get_set_lock);
    auto it = pictures.find(id);
    if(it != pictures.end())
    {
        if(state == ImageState::Last)
        {
            it->second.last = new_image;
            new_image->save(it->second.last_path,"png",100);
        }
        else
            it->second.origin = new_image;
        return true;
    }
    return false;
}

bool ImageFactory::set_image_state(const QString &id, ImageState state)
{
    QMutexLocker l(&image_get_set_lock);
    auto it = pictures.find(id);
    if(it != pictures.end())
    {
        it->second.state = state;
        return true;
    }
    return false;
}

bool ImageFactory::set_image_rui_hua_value(const QString &id, int value)
{
    QMutexLocker l(&image_get_set_lock);
    auto it = pictures.find(id);
    if(it != pictures.end())
    {
        //        if(it->second.Rui_Hua_integer == value)
        //            return false;
        //        else
        //      {
        it->second.Rui_Hua_integer = value;
        return true;
        //    }
    }
    return false;
}
float   brightness_and_contrast(float pix,float _Luminance_float, float _Contrast_float){
    float PI = 3.1415926;
    float B = _Luminance_float ;
    float c = _Contrast_float;
    float k = tan((45.0 + 44.0 * c) / 180.0 * PI);

    pix = ((pix*255.0 - 127.5 * (1.0 - B)) * k + 127.5 * (1.0 + B)) / 255.0;
    if(pix < 0)
        pix = 0;
    if(pix > 1.0)
        pix = 1.0;
    return pix;
}

bool ImageFactory::get_final_image(const QString &id, QImage &out,const QJsonObject & info)
{ 
    ImagesTable * tb = ImagesTable::getInstance();
    QString enable_fake_color_key = tb->get_field_name(ImagesTable::Field::Enable_Fake_Color_Filter_BOOLEAN);
    bool enable_fake_color = info[enable_fake_color_key].toBool();
    if(enable_fake_color)
        return get_final_image_fake_color(id, out, info);
    else
        return get_final_image_gray(id, out, info);
}

bool ImageFactory::get_final_image_fake_color(const QString &id, QImage &out, const QJsonObject &info)
{
    ImagesTable * tb = ImagesTable::getInstance();
    QString rotate_key = tb->get_field_name(ImagesTable::Field::Rotate_integer);
    QString brightness_key = tb->get_field_name(ImagesTable::Field::Luminance_float);
    QString const_key = tb->get_field_name(ImagesTable::Field::Contrast_float);
    QString gama_key = tb->get_field_name(ImagesTable::Field::Gama_float);

    QString float_key = tb->get_field_name(ImagesTable::Field::Float_float);
    QString rui_hua_key = tb->get_field_name(ImagesTable::Field::Rui_Hua_integer);
    QString enable_fake_color_key = tb->get_field_name(ImagesTable::Field::Enable_Fake_Color_Filter_BOOLEAN);
    QString enable_turn_key = tb->get_field_name(ImagesTable::Field::Enable_Turn_Color_Filter_BOOLEAN);

    QString filter_key = tb->get_field_name(ImagesTable::Field::Filter_Index_integer);
    QString enable_hor_mir = tb->get_field_name(ImagesTable::Field::Enable_Hor_Mirror_BOOLEAN);
    QString enable_ver_mir = tb->get_field_name(ImagesTable::Field::Enable_Ver_Mirror_BOOLEAN);

    QString begin_key = tb->get_field_name(ImagesTable::Field::Window_Begin_integer);
    QString end_key = tb->get_field_name(ImagesTable::Field::Window_End_integer);
    QString image_state_key = tb->get_field_name(ImagesTable::Field::Image_State_Integer);

    int rotate = info[rotate_key].toInt();
    float brightness = info[brightness_key].toDouble();
    float contrast = info[const_key].toDouble();
    float gama = info[gama_key].toDouble();
    float float_ = info[float_key].toDouble();
    int rui_hua = info[rui_hua_key].toInt();
    bool enable_fake_color = info[enable_fake_color_key].toBool();
    bool enable_turn = info[enable_turn_key].toBool();
    ImageFilter filter = ImageFilter(info[filter_key].toInt());
    bool enable_hor = info[enable_hor_mir].toBool();
    bool enable_ver = info[enable_ver_mir].toBool();
    double  begin = double(info[begin_key].toInt())/65535.0*255.0;
    double end = double(info[end_key].toInt())/65535.0*255.0;
    double dis = end - begin;
    ImageState image_state = ImageState(info[image_state_key].toInt());
    ImageFactory * image_factory = ImageFactory::get_instance();
    std::shared_ptr<QImage> pic = image_factory->get_image(id);
    if(!pic)
    {
        Log::i(TAG,"没有图片，取消导出:"+id);
        return false;
    }
    out = *pic;
    if(image_state == ImageState::Origin)
    {
        Log::i(TAG,"导出原图");
        return true;
    }
    else
    {
        QImage source = pic->convertToFormat(QImage::Format::Format_RGB888);
        out = source;
        int w = source.width();
        int h = source.height();
        cv::Mat src_cn3 =cv::Mat(h,w,CV_8UC3,source.bits(),source.bytesPerLine());
        cv::Mat target_cn3 =cv::Mat(h,w,CV_8UC3,out.bits(),out.bytesPerLine());


        float MIN_PIX = 0;
        float MAX_PIX = 255.0f;
        for(int y = 0; y < h;y++)
        {
            uchar* current_src_row =  src_cn3.ptr<uchar>(y);
            uchar* current_des_row =  target_cn3.ptr<uchar>(y);
            for(int x = 0; x < w ; x++)
            {
                uchar* src_pix =current_src_row + (x*3);
                uchar* des_pix = current_des_row +(x*3);

                float src_r  = src_pix[0];

                if(src_r < begin)
                    src_r = MIN_PIX;
                else if(src_r > end)
                    src_r = MAX_PIX;
                else
                {
                    src_r = float(src_r - begin)/float(dis)*MAX_PIX;
                }
                des_pix[0] =src_r;
                des_pix[1] =src_r;
                des_pix[2] =src_r;
            }
        }

        src_cn3  = target_cn3.clone();
        int b = (brightness - 1.0)*MAX_PIX;
        int c = (contrast - 1.0)*MAX_PIX;
        adjustBrightnessContrast8(src_cn3,target_cn3,b,c);

        if(float_ > 0.05)
        {
            src_cn3 = target_cn3.clone();
            QPointF TexSize(100.0, 100.0);
            for(int y = 0; y < h;y++)
                for(int x = 0; x < w ; x++)
                {
                    uchar* current_des_row =  target_cn3.ptr<uchar>(y);
                    uchar* des_pix = current_des_row +(x*3);
                    uchar des_pix_r = des_pix[0];

                    float normal_x = float(x)/float(w);
                    float normal_y = float(y)/float(h);
                    QPointF upLeftUV = QPointF(normal_x-1.0/TexSize.x()*(float_+0.1), normal_y-1.0/TexSize.y()*(float_+0.1));
                    QPoint offset = QPoint(upLeftUV.x()*w,upLeftUV.y()*h);
                    uchar offset_pix_r ;
                    if(offset.x() >= 0 && offset.x() < w && offset.y() >=0 && offset.y() < h)
                    {
                        uchar* current_src_row =  src_cn3.ptr<uchar>(offset.y());
                        uchar*  offset_pix = current_src_row+(offset.x()*3);
                        offset_pix_r  = offset_pix[0];
                    }
                    else
                    {
                        //                        cv::Mat zore1 = cv::Mat::zeros(3,3,CV_8UC3);
                        //                        cv::Mat zore2 = cv::Mat::zeros(3,3,CV_8UC3);
                        //                        adjustBrightnessContrast8(zore1,zore2,b,c);
                        offset_pix_r = des_pix_r;// zore2.ptr<uchar>(0)[0];
                    }

                    float result = des_pix_r  - offset_pix_r + MAX_PIX/2.0;
                    if(result < MIN_PIX)
                        result = MIN_PIX;
                    if(result > MAX_PIX)
                        result = MAX_PIX;
                    des_pix[0] = result;
                    des_pix[1] = result;
                    des_pix[2] = result;
                }
        }

        for(int y = 0; y < h;y++)
        {
            uchar* current_des_row =  target_cn3.ptr<uchar>(y);
            for(int x = 0; x < w ; x++)
            {
                uchar* des_pix = current_des_row +(x*3);
                if(enable_fake_color)
                {
                    float pix_r,pix_g,pix_b,temp = des_pix[0]/255.0f ;
                    if(enable_turn)
                        temp  = 1.0f - temp ;
                    if(temp < 63.0f/255.0f)
                    {
                        pix_r = 0.0f;
                        pix_g = (254.0f - 4.0*temp*255.0f)/255.0f;
                        pix_b = 1.0f;
                    }
                    else if(temp < 127.0f/255.0f){
                        pix_r = 0.0f;
                        pix_g = (-254.0f + 4.0f*temp*255.0f)/255.0f;
                        pix_b = (510.0f - 4.0f*temp*255.0f)/255.0f;
                    }
                    else if(temp < 191.0f/255.0f){
                        pix_r = (-510.0f + 4.0f*temp*255.0f)/255.0f;
                        pix_g = 1.0f;
                        pix_b = 0.0f;
                    }else{
                        pix_r = 1.0f;
                        pix_g = (1022.0f - 4.0f*temp*255.0f)/255.0f;
                        pix_b = 0.0f;
                    }
                    pix_r = pix_r * MAX_PIX;
                    pix_g = pix_g * MAX_PIX;
                    pix_b = pix_b * MAX_PIX;

                    if(pix_r < MIN_PIX)
                        pix_r = MIN_PIX;
                    if(pix_r > MAX_PIX)
                        pix_r = MAX_PIX;

                    if(pix_g < MIN_PIX)
                        pix_g = MIN_PIX;
                    if(pix_g > MAX_PIX)
                        pix_g = MAX_PIX;

                    if(pix_b < MIN_PIX)
                        pix_b = MIN_PIX;
                    if(pix_b > MAX_PIX)
                        pix_b = MAX_PIX;

                    pix_r =  std::pow(pix_r/MAX_PIX, 1.0/gama)*MAX_PIX;
                    pix_g =  std::pow(pix_g/MAX_PIX, 1.0/gama)*MAX_PIX;
                    pix_b =  std::pow(pix_b/MAX_PIX, 1.0/gama)*MAX_PIX;

                    if(pix_r < MIN_PIX)
                        pix_r = MIN_PIX;
                    if(pix_r > MAX_PIX)
                        pix_r = MAX_PIX;

                    if(pix_g < MIN_PIX)
                        pix_g = MIN_PIX;
                    if(pix_g > MAX_PIX)
                        pix_g = MAX_PIX;

                    if(pix_b < MIN_PIX)
                        pix_b = MIN_PIX;
                    if(pix_b > MAX_PIX)
                        pix_b = MAX_PIX;

                    des_pix[0] = pix_r;
                    des_pix[1] = pix_g;
                    des_pix[2] = pix_b;
                }
                else
                {
                    float pix_r = des_pix[0];
                    if(enable_turn)
                        pix_r = MAX_PIX - pix_r;
                    pix_r =  std::pow(pix_r/MAX_PIX, 1.0/gama)*MAX_PIX;
                    if(pix_r< MIN_PIX)
                        pix_r = MIN_PIX;
                    if(pix_r > MAX_PIX)
                        pix_r = MAX_PIX;
                    des_pix[0] = pix_r;
                    des_pix[1] = pix_r;
                    des_pix[2] = pix_r ;
                }
            }
        }


        cv::Mat src2 =target_cn3;
        cv::Mat ret;
        if(rotate == -90)
            rotate = 270;
        else if(rotate == -180)
            rotate = 180;
        else if (rotate == -270)
            rotate = 90;
        else if(rotate == -360)
            rotate = 0;

        if(rotate == 90)
        {
            cv::Mat temp;
            transpose(src2,ret);
            flip(ret,temp,1);
            ret = temp;
        }
        else if(rotate == 180)
        {
            cv::flip(src2,ret,-1);
        }
        else if(rotate == 270)
        {
            cv::Mat temp;
            transpose(src2,ret) ;
            flip(ret,temp,0);
            ret = temp;
        }
        else
        {
            if(enable_hor || enable_ver)
                out =out.mirrored(enable_hor,enable_ver);
            return true;
        }

        out = QImage(ret.data, ret.cols,ret.rows,3*sizeof (uchar) *ret.cols,QImage::Format::Format_RGB888).copy();
        if(enable_hor || enable_ver)
            out =out.mirrored(enable_hor,enable_ver);
        return true;
    }
}

bool ImageFactory::get_final_image_gray(const QString &id, QImage &out, const QJsonObject &info)
{
    ImagesTable * tb = ImagesTable::getInstance();
    QString rotate_key = tb->get_field_name(ImagesTable::Field::Rotate_integer);
    QString brightness_key = tb->get_field_name(ImagesTable::Field::Luminance_float);
    QString const_key = tb->get_field_name(ImagesTable::Field::Contrast_float);
    QString gama_key = tb->get_field_name(ImagesTable::Field::Gama_float);

    QString float_key = tb->get_field_name(ImagesTable::Field::Float_float);
    QString rui_hua_key = tb->get_field_name(ImagesTable::Field::Rui_Hua_integer);
    QString enable_fake_color_key = tb->get_field_name(ImagesTable::Field::Enable_Fake_Color_Filter_BOOLEAN);
    QString enable_turn_key = tb->get_field_name(ImagesTable::Field::Enable_Turn_Color_Filter_BOOLEAN);

    QString filter_key = tb->get_field_name(ImagesTable::Field::Filter_Index_integer);
    QString enable_hor_mir = tb->get_field_name(ImagesTable::Field::Enable_Hor_Mirror_BOOLEAN);
    QString enable_ver_mir = tb->get_field_name(ImagesTable::Field::Enable_Ver_Mirror_BOOLEAN);

    QString begin_key = tb->get_field_name(ImagesTable::Field::Window_Begin_integer);
    QString end_key = tb->get_field_name(ImagesTable::Field::Window_End_integer);
    QString image_state_key = tb->get_field_name(ImagesTable::Field::Image_State_Integer);

    int rotate = info[rotate_key].toInt();
    float brightness = info[brightness_key].toDouble();
    float contrast = info[const_key].toDouble();
    float gama = info[gama_key].toDouble();
    float float_ = info[float_key].toDouble();
    int rui_hua = info[rui_hua_key].toInt();
    bool enable_fake_color = info[enable_fake_color_key].toBool();
    bool enable_turn = info[enable_turn_key].toBool();
    ImageFilter filter = ImageFilter(info[filter_key].toInt());
    bool enable_hor = info[enable_hor_mir].toBool();
    bool enable_ver = info[enable_ver_mir].toBool();
    float begin = info[begin_key].toInt();
    float end = info[end_key].toInt();
    float dis = end - begin;
    ImageState image_state = ImageState(info[image_state_key].toInt());
    ImageFactory * image_factory = ImageFactory::get_instance();
    std::shared_ptr<QImage> pic = image_factory->get_image(id);
    if(!pic)
    {
        Log::i(TAG,"没有图片，取消导出:"+id);
        return false;
    }
    out = *pic;
    if(image_state == ImageState::Origin)
    {
        Log::i(TAG,"导出原图");
        return true;
    }
    else
    {
        out.fill(QColor("#00000000"));
        QPointF TexSize(100.0, 100.0);
        float MIN_PIX = 0;
        float MAX_PIX = 65535.0f;
        for(int y = 0; y < pic->height();y++)
        {
            quint16 * current_row_source_ptr =(quint16 *)( pic->bits() + (y*pic->bytesPerLine()));
            quint16 * current_row_target_ptr =(quint16 *)( out.bits() + (y*out.bytesPerLine()));
            for(int x =0; x< pic->width();x++)
            {
                float pix = current_row_source_ptr[x];
                if(pix < begin)
                    pix = MIN_PIX;
                else if(pix > end)
                    pix = MAX_PIX;
                else
                {
                    pix = float(pix - begin)/float(dis)*MAX_PIX;
                }
                current_row_target_ptr[x] =quint16(pix);
            }
        }
        out = out.convertToFormat(QImage::Format::Format_Grayscale8);
        cv::Mat src =cv::Mat(out.height(),out.width(),CV_8UC1,out.bits(),out.bytesPerLine());
        cv::Mat temp_src = src.clone();
        int b = (brightness - 1.0)*255;
        int c = (contrast - 1.0)*255;
        adjustBrightnessContrast8(temp_src,src,b,c);
        out = out.convertToFormat(QImage::Format::Format_Grayscale16);

        if(float_ > 0.05)
        {
            QImage clone_out = out.copy();
            for(int y = 0; y < out.height();y++)
            {
                quint16 * current_row_target_ptr =(quint16 *)( out.bits() + (y*out.bytesPerLine()));
                for(int x =0; x< out.width();x++)
                {
                    float pix = current_row_target_ptr[x];
                    float normal_x = float(x)/float(pic->width());
                    float normal_y = float(y)/float(pic->height());
                    QPointF upLeftUV = QPointF(normal_x-1.0/TexSize.x()*(float_+0.1), normal_y-1.0/TexSize.y()*(float_+0.1));
                    QPoint offset = QPoint(upLeftUV.x()*out.width(),upLeftUV.y()*out.height());
                    quint16 offset_pix;
                    if(offset.x() >= 0 && offset.x() < out.width() && offset.y() >=0 && offset.y() < out.height())
                    {
                        quint16 * this_row =(quint16 *)( clone_out.bits() + (offset.y()*clone_out.bytesPerLine()));
                        offset_pix =this_row[offset.x()];
                    }
                    else
                    {
                        //                        cv::Mat zore1 = cv::Mat::zeros(3,3,CV_8UC3);
                        //                        cv::Mat zore2 = cv::Mat::zeros(3,3,CV_8UC3);
                        //                        adjustBrightnessContrast8(zore1,zore2,b,c);
                        offset_pix = pix;//float(zore2.ptr<uchar>(0)[0])/255.0f * 65535.0f;
                    }
                    pix = pix - offset_pix + 65535.0f/2;
                    if(pix < 0)
                        pix = 0;
                    if(pix > 65535.0f)
                        pix = 65535.0f;
                    current_row_target_ptr[x] = pix;
                }
            }
        }

        for(int y = 0; y < out.height();y++)
        {
            quint16 * current_row_target_ptr =(quint16 *)( out.bits() + (y*out.bytesPerLine()));
            for(int x =0; x< out.width();x++)
            {
                float pix = current_row_target_ptr[x];
                if(enable_turn)
                {
                    pix = MAX_PIX - pix;
                }

                pix =  std::pow(pix/MAX_PIX, 1.0/gama)*MAX_PIX;
                if(pix < 0)
                    pix = 0;
                if(pix > MAX_PIX)
                    pix = MAX_PIX;
                current_row_target_ptr[x]= pix;
            }
        }
        cv::Mat src2 =cv::Mat(out.height(),out.width(),CV_16UC1,out.bits(),out.bytesPerLine());
        cv::Mat ret;
        if(rotate == -90)
            rotate = 270;
        else if(rotate == -180)
            rotate = 180;
        else if (rotate == -270)
            rotate = 90;
        else if(rotate == -360)
            rotate = 0;

        if(rotate == 90)
        {
            cv::Mat temp;
            transpose(src2,ret);
            flip(ret,temp,1);
            ret = temp;
        }
        else if(rotate == 180)
        {
            cv::flip(src2,ret,-1);
        }
        else if(rotate == 270)
        {
            cv::Mat temp;
            transpose(src2,ret) ;
            flip(ret,temp,0);
            ret = temp;
        }
        else
        {
            if(enable_hor || enable_ver)
                out =out.mirrored(enable_hor,enable_ver);
            return true;
        }

        out = QImage(ret.data, ret.cols,ret.rows,sizeof(quint16)*ret.cols,QImage::Format::Format_Grayscale16).copy();
        if(enable_hor || enable_ver)
            out =out.mirrored(enable_hor,enable_ver);
        return true;
    }
}

