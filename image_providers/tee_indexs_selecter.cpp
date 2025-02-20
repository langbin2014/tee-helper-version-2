#include "tee_indexs_selecter.h"
#include "c++/log.h"
#include "c++/tools.h"
#include<QPainter>
TeeIndexsSelecterImageProvider::TeeIndexsSelecterImageProvider():QQuickImageProvider(QQuickImageProvider::Image)
{
    //
    //QImage::pixel: coordinate (690,59) out of range
    //  QImage::pixel: coordinate (690,66) out of range
}

QImage TeeIndexsSelecterImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QMutexLocker l (&mutex);
    //成人第一行 y∈43,90,第二行 127-172
    //0-25,40,68,
    // qDebug() <<id << *_current_selected_iamge;
    if(size)
        *size = _current_selected_iamge->size();
    _dirty = false;
    return *_current_selected_iamge;
}

QString TeeIndexsSelecterImageProvider::get_url()
{

    return QString::number(Tools::get_1970_ms());

}

void TeeIndexsSelecterImageProvider::select(const QString &tee_type)
{
    QMutexLocker l (&mutex);
    if(tee_type == "adult")
    {
        _current_selected_iamge = _adult;
        _current_checkeds = &_adult_checkeds;
        _current_rect = &_adult_rects;
        _current_table = _adult_table;
        _current_unchecked_images=&_adult_unchecked_images;
        _current_checked_images=&_adult_checked_images;

    }
    else if(tee_type == "kid")
    {
        _current_selected_iamge = _baby;
        _current_checkeds = &_baby_checkeds;
        _current_rect = &_baby_rects;
        _current_table = _baby_table;
        _current_unchecked_images=&_baby_unchecked_images;
        _current_checked_images=&_baby_checked_images;
    }
    else {
        Log::e(TAG,"参数错误:"+tee_type);
        throw "参数错误:"+tee_type;
    }
}

void TeeIndexsSelecterImageProvider::reinit(const QString &tee_type, const QString &tee_indexs)
{
    QJsonArray indexs;
    Tools::str_2_jsonarray(tee_indexs,indexs);
    if(tee_type == "adult")
    {
        for(int i =0; i<_baby_checkeds.size();i++)
            _baby_checkeds[i] = false;
        for(int i =0; i<_adult_checkeds.size();i++)
            _adult_checkeds[i] = false;
        for(auto it = indexs.begin();it!= indexs.end();it++)
        {
            int i = it->toInt();
            _adult_checkeds.at(i) =  true;
        }
    }
    else if(tee_type == "kid")
    {
        for(int i =0; i<_baby_checkeds.size();i++)
            _baby_checkeds[i] = false;
        for(int i =0; i<_adult_checkeds.size();i++)
            _adult_checkeds[i] = false;
        for(auto it = indexs.begin();it!= indexs.end();it++)
        {
            int i = it->toInt();
            _baby_checkeds.at(i) =  true;
        }
    }
    else
    {
        Log::e(TAG,"参数错误:"+tee_type);
        throw "参数错误:"+tee_type;
    }

    {
        QPainter p;
        p.begin(_adult);
        for(int i =0;i <_adult_rects.size();i++)
        {
            QImage * pic = _adult_checkeds.at(i) ? _adult_checked_images.at(i) : _adult_unchecked_images.at(i);
            const QRect & rect = _adult_rects.at(i);
            p.drawImage(rect,*pic);
        }
        p.end();
    }

    {
        QPainter p;
        p.begin(_baby);
        for(int i =0;i <_baby_rects.size();i++)
        {
            QImage * pic = _baby_checkeds.at(i) ? _baby_checked_images.at(i) : _baby_unchecked_images.at(i);
            const QRect & rect = _baby_rects.at(i);
            p.drawImage(rect,*pic);
        }
        p.end();
    }
    select(tee_type);
    _dirty = true;
}

void TeeIndexsSelecterImageProvider::mouse_event(float x_, float y_, const QString &action)
{
    QMutexLocker l (&mutex);
    const static QColor checked_color  = QColor("#ff1984FC");
    const static QColor not_checked_color  = QColor("#ffffffff");
    int x = x_ * _current_table->width();
    int y = y_ * _current_table->height();
    QColor color(_current_table->pixel(x,y));
    if(color.red() > 14 ||  color.green() > 14 || color.blue() > 14)
    {
        for(int i =0;i < _current_rect->size();i++)
        {
            const QRect & rect =_current_rect->at(i);
            if(rect.contains(x,y))
            {
                _dirty = true;
                bool old_checked = _current_checkeds->at(i);
                bool new_checked = !old_checked;
                (*_current_checkeds)[i] = new_checked;
                QImage * result_image = new_checked ? _current_checked_images->at(i) : _current_unchecked_images->at(i);
                {
                    QPainter p;
                    p.begin(_current_selected_iamge);
                    p.drawImage(rect,*result_image);
                    p.end();
                    return;
                }/*
                for(int x1 = rect.x(),x=0;x1 < rect.x()+rect.width();x1++,x++)
                    for(int y1 = rect.y(),y=0;y1 < rect.y()+rect.height();y1++,y++)
                    {
                        QColor new_color(result_image->pixel(x,y));
                         if(new_color.red() > 14 ||  new_color.green() > 14 || new_color.blue() > 14)
                             _current_selected_iamge->setPixel(x1,y1,new_color.rgba());
                    }
                return ;
                if(action == "c") //点击
                    (*_current_checkeds)[i] = new_checked;
                QColor result_color = new_checked || action == "m" ? checked_color : not_checked_color;
                QColor current_color(_current_selected_iamge->pixel(x,y));
                if(result_color == current_color)
                {//颜色已经改好了
                    _dirty = false;
                    return ;
                }
                for(int x1 = rect.x();x1 < rect.x()+rect.width();x1++)
                    for(int y1 = rect.y();y1 < rect.y()+rect.height();y1++)
                    {
                        QColor temp_color(_current_selected_iamge->pixel(x1,y1));
                        if(temp_color.red() > 14 ||  temp_color.green() > 14 || temp_color.blue() > 14)
                        {
//                            if(temp_color == result_color)
//                            { //颜色已经改好了
//                                _dirty = false;
//                                return;
//                            }
                            _current_selected_iamge->setPixel(x1,y1,result_color.rgba());

                        }
                    }
                return;
                */
            }
        }
    }
}

//void TeeIndexsSelecterImageProvider::click(float x, float y)
//{

//}

//QString TeeIndexsSelecterImageProvider::collecting_collecting_tee_type() const
//{
//    return _collecting_collecting_tee_type;
//}


//void TeeIndexsSelecterImageProvider::setCollecting_collecting_tee_type(const QString &collecting_collecting_tee_type)
//{
//        _collecting_collecting_tee_type = collecting_collecting_tee_type;

//}

//QJsonArray TeeIndexsSelecterImageProvider::collecting_tee_indexs() const
//{
//    return _collecting_tee_indexs;
//}

//void TeeIndexsSelecterImageProvider::setCollecting_tee_indexs(const QJsonArray &collecting_tee_indexs)
//{
//    _collecting_tee_indexs = collecting_tee_indexs;
//}

bool TeeIndexsSelecterImageProvider::dirty()
{
    return _dirty;
}

void TeeIndexsSelecterImageProvider::init()
{
    _adult_table  = new QImage(":/images/audlt_tees_checked.png");
    _baby_table  = new QImage(":/images/baby_tees_checked.png");
    _adult = new QImage(":/images/audlt_tees_unchecked.png");
    _baby = new QImage(":/images/baby_tees_unchecked.png");
    init(_adult,43,90,127,172,_adult_rects);
    if(DEBUG)
        qDebug()<<"_adult_rects" <<_adult_rects.size();
    //成人第一行 y∈43,90,第二行 127-172;
    init(_baby,40,88,124,173,_baby_rects);
    //婴儿第一行 y∈40,88,第二行 124-173;
    _adult_checkeds.resize(_adult_rects.size(),false);
    _baby_checkeds.resize(_baby_rects.size(),false);

    for(int i = 0; i < _adult_rects.size();i++)
    {
        QImage * unchecked = new QImage;
        *unchecked = _adult->copy(_adult_rects[i]);
        _adult_unchecked_images.push_back(unchecked);

        QImage * checked = new QImage;
        *checked = _adult_table->copy(_adult_rects[i]);
        _adult_checked_images.push_back(checked);

    }

    for(int i = 0; i < _baby_rects.size();i++)
    {
        QImage * unchecked = new QImage;
        *unchecked = _baby->copy(_baby_rects[i]);
        QImage * checked = new QImage;
        *checked = _baby_table->copy(_baby_rects[i]);
        _baby_checked_images.push_back(checked);
        _baby_unchecked_images.push_back(unchecked);
    }

    //    _current_checkeds = &_adult_checkeds;
    //    _current_rect = & _adult_rects;
    //    _current_table = _adult_table;
    //    _current_selected_iamge = _adult;
    select("adult");
    if(DEBUG)
        qDebug() << _adult_rects <<"\n" << _baby_rects << ","<<_adult_rects.size();
    //    for(int x = 0 ;x < _adult->width();x++)
    //        for(int y = 43; y < 172;y++)
    //        {
    //            QColor color(_adult->pixel(x,y));
    //            if(color.red() > 14 ||  color.green() > 14 || color.blue() > 14)
    //            {
    //                _adult->setPixel(x,y,qRgba(255,255,255,255));
    //            }
    //        }

    //    for(int x = 0 ;x < _baby->width();x++)
    //        for(int y = 40; y < 173;y++)
    //        {
    //            QColor color(_baby->pixel(x,y));
    //            if(color.red() > 14 ||  color.green() > 14 || color.blue() > 14)
    //            {
    //                _baby->setPixel(x,y,qRgba(255,255,255,255));
    //            }
    //        }
}

void TeeIndexsSelecterImageProvider::get_database_indexs_format(QJsonArray &out_indexs)
{
    QMutexLocker l (&mutex);
    if(out_indexs.size() > 0)
        out_indexs = QJsonArray();
    for(int i=0; i <_current_checkeds->size();i++)
        if(_current_checkeds->at(i) == true)
            out_indexs.push_back(QJsonValue(i));
}

void TeeIndexsSelecterImageProvider::init(QImage* pic,int y1,int y2,int y3,int y4,std::vector<QRect> & out)
{//成人第一行 y∈43,90,第二行 127-172
    std::vector<int> indexs;
    std::vector<bool> last_state;
    for(int x = 0; x < pic->width();x++)
    {
        bool found_color = false;
        for(int y = y1;y < y2;y++)
        {
            QColor color(pic->pixel(x,y));
            if(color.red() > 14 ||  color.green() > 14 || color.blue() > 14)
            {
                found_color = true;
                break;
            }
        }
        if( last_state.size()== 0)
        {
            if(found_color)
            {
                last_state.push_back(found_color);
                indexs.push_back(x);
            }
        }
        else
        {
            bool last = last_state.at(last_state.size()-1);
            if(last != found_color)
            {
                last_state.push_back(found_color);
                indexs.push_back(x);
            }
        }
    }
    if(DEBUG)
        qDebug() << indexs;
    if(indexs.size() % 2 != 0)
        indexs.push_back(pic->width());

    for(int i =0; i <indexs.size();i+=2 )
    {
        int x = indexs.at(i);
        int width = indexs.at(i+1) - x;
        int y = y1;
        int height = y2 - y1;
        out.push_back(QRect(x,y,width,height));
    }

    for(int i = 0; i< indexs.size();i+=2 )
    {
        int x = indexs.at(i);
        int width = indexs.at(i+1) - x;
        int y = y3;
        int height = y4 - y3;
        out.push_back(QRect(x,y,width,height));
    }

}



