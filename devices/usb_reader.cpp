#include <dshow.h>
#include <iostream>
#include <objbase.h>
#include <strmif.h>
#include <vidcap.h>         // For IKsTopologyInfo
#include <ksproxy.h>        // For IKsControl
#include <ks.h>
#include <ksmedia.h>
#include <Windows.h>
#include "usb_reader.h"
#include "c++/log.h"
#include"c++/managers/pcture_manager.h"
#include"c++/managers/patient_manager.h"
#include "c++/tools.h"
#include<QFile>
#include"c++/settings.h"
#include<QStorageInfo>
QString UsbReader::TAG = "usb传感器采集类";
bool UsbReader::DEBUG_ = true;
UsbReader::UsbReader(QObject * parent):QThread(parent),_run(false)
{

}

void UsbReader::stop()
{
    _run = false;
}

void UsbReader::begin_cap(UsbReader::Mode1 mode, UsbReader::Bin2 bin, UsbReader::Filter3 filter)
{
    _cmd = get_mode_setting(mode,bin,filter); //已经去掉立刻出图的功能，使用自动出图功能
    _run = true;
    start();
}

void UsbReader::get_usb_info()
{
    auto f = [this](){
        this->get_version_and_ser_num();
    };
    task = f;
}

 bool UsbReader::check_fixed_file_is_ok(){
    // 获取所有硬盘信息
    auto pm = PatientManager::get_instance()->get_picture_manager();
    QString fixed_dark_file = pm->get_dark_pic_dir();
    QString fixed_light_file = pm->get_light_pic_dir();
    QString fixed_dark_file_factory = pm->get_factory_dark_pic_dir();
    QString fixed_light_file_factory = pm->get_factory_light_pic_dir();

    QFile dark(fixed_dark_file);
    QFile light(fixed_light_file);
    QFile dark_factory(fixed_dark_file_factory);
    QFile light_factory(fixed_light_file_factory);


    if(dark.exists() && light.exists())
        return true;

    if(dark_factory.exists() && light_factory.exists())
    {
       // if(dark.exists() == false)
        {
            dark_factory.open(QIODevice::ReadOnly);
            dark.open(QIODevice::WriteOnly);
            dark.write(dark_factory.readAll());
        }

       // if(light.exists() == false)
        {
            light_factory.open(QIODevice::ReadOnly);
            light.open(QIODevice::WriteOnly);
            light.write(light_factory.readAll());
        }
        return true;
    }


    QList<QStorageInfo> storageList = QStorageInfo::mountedVolumes();



    foreach (const QStorageInfo& storage, storageList) {
        if ( storage.isValid()) {

            QString name = storage.device();
            QString displayName = storage.displayName();
            QString fileSystemType = storage.fileSystemType();


            qDebug() << "硬盘名称：" << name;
            qDebug() << "硬盘显示名称：" << displayName;
            qDebug() << "文件系统类型：" << fileSystemType;


            QString root_path =  storage.rootPath() + "/Correct";
            QString check_dark_url =  pm->get_usb_dark_pic_dir(root_path);
            QString check_light_url =  pm->get_usb_light_pic_dir(root_path);

            QFile check_dark(check_dark_url);
            QFile check_light(check_light_url);
            if(check_dark.exists() && check_light.exists())
            {
               // if(dark.exists() == false)
                {
                    check_dark.open(QIODevice::ReadOnly);
                    dark.open(QIODevice::WriteOnly);
                    dark.write(check_dark.readAll());
                }

               // if(light.exists() == false)
                {
                    check_light.open(QIODevice::ReadOnly);
                    light.open(QIODevice::WriteOnly);
                    light.write(check_light.readAll());
                }

                pm->show_message("校准文件从U盘成功导入","校准文件从U盘成功导入");
                return true;
            }
        }
    }

    return false;
}

void UsbReader::run()
{
    msleep(2000);
    auto pm = PatientManager::get_instance()->get_picture_manager();

    while(pm->get_dont_show_usb_info() == false)
    {
        get_version_and_ser_num();

        QString vs = pm->get_usb_version();
        if(vs == "")
        {
            emit  pm->no_usb_found();
            QThread::msleep(1000);
            continue;
        }

        bool ok = check_fixed_file_is_ok();

        if(ok)
            break;
        else
        {
            emit pm->no_fixed_file_found();
            QThread::msleep(1000);
            continue;
        }

    }



    auto settings = Settings::get_instance();
    while(_run)
    {

        auto temp = task;
        task = nullptr;
        if(temp != nullptr)
        {
            temp();
        }
        if(pm->get_show_fixed_page()) //校准模式
        {
            if(pm->get_fixed_mode_starting()) //开始校准
            {
                pm->get_fixed_image_provider()->set_dark_image_data(QByteArray());
                pm->get_fixed_image_provider()->set_light_image_data(QByteArray());
                pm->set_cuurrent_pic_is_lighting(false);
                bool l = pm->get_cuurrent_pic_is_lighting(); //亮图或暗图
                pm->new_state(0,true,"");

                Mode1 m =  Mode1::AV;
                _cmd = get_mode_setting(m,UsbReader::Bin2::NoBining,UsbReader::Filter3(1)); //已经去掉立刻出图的功能，使用自动出图功能
                buf = QByteArray();
                pm->new_state(1,true,"");
                bool ok = run_fixed();

                if(!ok) //未链接
                {
                    pm->new_state(2,false,settings->tran("没有发现传感器"));
                    pm->set_fixed_mode_starting(false);
                    QThread::msleep(100);//睡眠5秒
                    continue;
                }else
                {
                    if(buf.size() > 0){
                        pm->get_fixed_image_provider()->set_dark_image_data(buf);

                        double av;
                        int ret =check_fix_dark_pic_is_ok(av);
                        pm->set_avager_ct_value_dark(av);
                        if(ret == 0)
                        {
                            //                            QString d = QCoreApplication::applicationDirPath() + "/fixed/dark.raw";
                            //                            QFile   file(d);
                            //                            bool ok = file.open(QIODevice::WriteOnly);
                            bool ok = pm->save_dark_image(buf);
                            if(!ok)
                            {
                                pm->new_state(2,false,settings->tran("写入文件失败"));
                                pm->set_fixed_mode_starting(false);
                                QThread::msleep(100);//睡眠5秒
                                continue;
                            }
                            else{
                                //  file.write(buf);
                                pm->new_state(2,true,"");

                            }
                        }
                        else if(ret == -1)
                        {
                            pm->new_state(2,false,settings->tran("设备异常，出暗图CT值不应该过低"));
                            pm->set_fixed_mode_starting(false);
                            QThread::msleep(100);//睡眠5秒
                            continue;
                        }
                        else
                        {
                            pm->new_state(2,false,settings->tran("设备异常，出暗图CT值不应该过高"));
                            pm->set_fixed_mode_starting(false);
                            QThread::msleep(100);//睡眠5秒
                            continue;
                        }
                    }
                    else {
                        pm->new_state(2,false,settings->tran("用户终止了操作"));
                        pm->set_fixed_mode_starting(false);
                        QThread::msleep(100);//睡眠5秒
                        continue;
                    }
                }

                {
                    QThread::msleep(1000);//睡眠5秒
                    pm->set_cuurrent_pic_is_lighting(true);
                    pm->new_state(3,true,"");

                    Mode1 m =   Mode1::VTC ;
                    _cmd = get_mode_setting(m,UsbReader::Bin2::NoBining,UsbReader::Filter3(1)); //已经去掉立刻出图的功能，使用自动出图功能
                    buf = QByteArray();
                    ok = run_fixed();

                    pm->new_state(4,true,"");
                    if(buf.size() > 0)
                    {

                        pm->get_fixed_image_provider()->set_light_image_data(buf);
                        double av;
                        int ret =check_fix_light_pic_is_ok(av);
                        pm->set_avager_ct_value_light(av);
                        if(ret == 0)
                        {

                            //                            QString l = QCoreApplication::applicationDirPath() + "/fixed/light.raw";
                            //                            QFile   file(l);
                            //                            bool ok =  file.open(QIODevice::WriteOnly);
                            bool ok = pm->save_light_image(buf);
                            if(!ok)
                            {
                                // pm->show_message("","写入文件失败");
                                pm->new_state(5,false,settings->tran("写入文件失败"));
                                pm->set_fixed_mode_starting(false);
                                QThread::msleep(100);//睡眠5秒
                                continue;
                            }
                            else{
                                //  file.write(buf);
                                pm->new_state(5,true,"");

                            }
                        }
                        else if(ret == -1)
                        {
                            pm->new_state(5,false,settings->tran("不及格，请加大X光剂量重拍明图"));
                            pm->set_fixed_mode_starting(false);
                            QThread::msleep(100);//睡眠5秒
                            continue;
                        }
                        else
                        {
                            //   pm->show_message("","不及格，请减小X光剂量重拍明图");
                            pm->new_state(5,false,settings->tran("不及格，请减小X光剂量重拍明图"));
                            pm->set_fixed_mode_starting(false);
                            QThread::msleep(100);//睡眠5秒
                            continue;
                        }

                    }
                    else
                    {
                        pm->new_state(5,false,settings->tran("拍照超时或者其他不明原因"));
                        pm->set_fixed_mode_starting(false);
                        QThread::msleep(100);//睡眠5秒
                        continue;
                    }
                    QThread::msleep(100);
                }
                pm->set_fixed_mode_starting(false);//退出校准图片采集,需要重新点开始
            }

            QThread::msleep(100);

        }

        else
        {
            if(pm->get_usb_reading())
            {


                _cmd = get_mode_setting(Mode1::VTC,UsbReader::Bin2::NoBining,UsbReader::Filter3(1));
                bool ok = run_2();
                pm->set_usb_reading(false);
                if(!ok) //未链接
                {
                    qDebug() <<"未链接";
                    QThread::msleep(5000);//睡眠5秒
                }
                else
                    QThread::msleep(100);
            }
            else
                QThread::msleep(100);

        }

    }
}

int UsbReader::get_mode_setting(UsbReader::Mode1 mode, UsbReader::Bin2 bin, UsbReader::Filter3 filter)
{
    int frameMode = int(mode) * 0x2 + int(bin) * 0x4 + int(filter) * 0x8 + 1;
    return frameMode;
}
static  inline  int swapInt32(int value)
{
    return ((value & 0x000000FF) << 24) |
            ((value & 0x0000FF00) << 8) |
            ((value & 0x00FF0000) >> 8) |
            ((value & 0xFF000000) >> 24);
}

bool UsbReader::run_2()
{
    QTime time;
    time.start();
    auto pm = PatientManager::get_instance()->get_picture_manager();

    HRESULT ret = CoInitialize(NULL);
    bool is_connected = false;
    if (FAILED(ret))
    {
        //_run = false;
        emit error(tr("失败1"));
        goto  end;
    }
    else
        qDebug() << "ok-1"<< "+" << time.restart();

    IGraphBuilder* pGraph = nullptr;
    ICaptureGraphBuilder2* pBuild = nullptr;
    //创建用于抓拍的Sample Grabber Filter.
    IBaseFilter* pGrabberStill = NULL;
    ISampleGrabber* pSampleGrabberStill = NULL;
    IBaseFilter* pNull = nullptr;
    IMediaControl* ctrl = nullptr; //控制
    IBaseFilter* out__filter = nullptr;
    IMoniker* selected_imoniker = nullptr;
    bool ok = InitCaptureGraphBuilder(&pGraph, &pBuild);
    if (!ok)
    {
        Log::e(TAG, "失败2");
        emit error(tr("失败2"));
        goto end;
    }
    else
        qDebug() << "ok-2"<< "+" << time.restart();

    ok = init_yuv_receiver(&pGrabberStill, &pSampleGrabberStill);
    if (!ok)
    {
        Log::e(TAG, "失败3");
        emit error(tr("失败3"));
        goto end;
    }
    else
        qDebug() << "ok-3"<< "+" << time.restart();

    ret = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pNull);
    if (FAILED(ret))
    {
        Log::e(TAG,"创建隐藏窗口过滤器失败4");
        emit error(tr("创建隐藏窗口过滤器失败4"));
        goto end;
    }
    else
        qDebug() << "ok-4"<< "+" << time.restart();
    ret = pGraph->QueryInterface(IID_IMediaControl, (LPVOID*)&ctrl);
    if (FAILED(ret))
    {
        Log::e(TAG, "失败5");
        emit error(tr("失败5"));
        goto end;
    } else
        qDebug() << "ok-5"<< "+" << time.restart();

    ok = get_input_filter(std::string("RGBIC UVC LION"), &out__filter, &selected_imoniker); //MKL Camera ,RGBIC UVC LION
    if (!ok || out__filter == nullptr)
    {
        Log::e(TAG,"失败6");
        emit error(tr("失败6"));
        goto end;
    } else
        qDebug() << "ok-6"<< "+" << time.restart();

    emit connected();
    is_connected = true;

    //开始构建预览graph
    //加入设备抓拍filter
    ret = pGraph->AddFilter(out__filter, L"Video Capture");
    if (FAILED(ret))
    {
        out__filter->Release();
        out__filter = nullptr;
        Log::e(TAG,"失败7");
        emit error(tr("失败7"));
        goto end;
    } else
        qDebug() << "ok-7"<< "+" << time.restart();

    //加入DirectShow中自带的SampleGrabber Filter
    ret = pGraph->AddFilter(pGrabberStill, L"Still Sample Grabber");
    if (FAILED(ret))
    {
        Log::e(TAG,"失败8");
        pGrabberStill->Release();
        pGrabberStill = nullptr;
        emit error(tr("失败8"));
        goto end;
    } else
        qDebug() << "ok-8"<< "+" << time.restart();

    //加入DirectShow中自带的NullRender Filter
    ret = pGraph->AddFilter(pNull, L"NullRender");
    if (FAILED(ret))
    {
        Log::e(TAG,"失败9");
        pNull->Release();
        pNull = nullptr;
        emit error(tr("失败9"));
        goto end;
    }
    else
        qDebug() << "ok-9"<< "+" << time.restart();
    /*
    //加入DirectShow中自带的SampleGrabber Filter
    hr = pGraph->AddFilter(pGrabberF, L"Sample Grabber");
    if (FAILED(hr))
    {
        printf("Couldn't add sample grabber to graph!  hr=0x%x\n", hr);
        // Return an error.
    }
    */

    //使用Capture Graph Builder构建预览 Graph

    ret = pBuild->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, out__filter, pGrabberStill, pNull);
    if (FAILED(ret))
    {
        Log::e(TAG,"失败10");
        out__filter->Release();
        out__filter = nullptr;
        pGrabberStill->Release();
        pGrabberStill = nullptr;
        pNull->Release();
        pNull = nullptr;
        emit error(tr("失败10"));
        goto end;
    } else
        qDebug() << "ok-10"<< "+" << time.restart();

    //configure the Sample Grabber so that it buffers samples :
    ret = pSampleGrabberStill->SetOneShot(true);
    ret = pSampleGrabberStill->SetBufferSamples(false);


    //获取设备输出格式信息
    AM_MEDIA_TYPE mt;
    ret = pSampleGrabberStill->GetConnectedMediaType(&mt);
    if (FAILED(ret))
    {
        Log::e(TAG,"失败11");
        emit error(tr("失败11"));
        goto end;
    } else
        qDebug() << "ok-11"<< "+" << time.restart();

    qDebug() << (mt.subtype == MEDIASUBTYPE_NV12) << "," << (mt.subtype == MEDIASUBTYPE_YV12) << "," << (mt.subtype == MEDIASUBTYPE_YUY2);
    VIDEOINFOHEADER* vih = (VIDEOINFOHEADER*)mt.pbFormat;
    qDebug() << "width" << vih->bmiHeader.biWidth << "height" << vih->bmiHeader.biHeight << Qt::endl;
    CSampleGrabberCB* CB = new CSampleGrabberCB();

    CB->set_data_listener([this](BYTE * buf, long size){
        //TODO 数据格式为YUY2，需要转换
        int len = 1660 * 2280* 2;
        qDebug() << "样本" << size << "所需" << len;
        // char FileName[256] = "data.yuv";
        //	sprintf_s(FileName, "data.yuv");
        // FILE * out = fopen(FileName, "wb");
        if(size < len)
        {
            Log::e(TAG,QString("usb 数据长度不对:%1").arg(size));
            emit error(QString(tr("usb 数据长度不对:")+ "%1").arg(size));
            _got_picture = true;
            emit got_picture(nullptr);
            return;
        }
        QByteArray * data = new QByteArray();//(const char *)buf, len);
        *data = QByteArray((const char *)buf, len);
        emit got_picture(data);
        _got_picture = true;
        //fwrite(pBuffer, 1, len, out);
        //   fclose(out);
    });

    //设置触发抓拍后，调用的回调函数 0-调用SampleCB 1-BufferCB
    ret = pSampleGrabberStill->SetCallback(CB, 1);
    long now = get_brightness(out__filter);

    pm->set_usb_version( get_version(out__filter));
    pm->set_usb_ser_num(get_ser_number(out__filter));

    //    if (now != 0)
    //    {
    qDebug() << "重置usb:" << set_brightness(out__filter, 0);
    QThread::msleep(100);
    //   }
    qDebug() << "set_brightness" << set_brightness(out__filter, _cmd);
    QThread::msleep(100);
    qDebug() << "get_brightness" << get_brightness(out__filter);
    int count = 0;
    qDebug() << "等待拍照"<< "-" << time.restart();
    _got_picture = false;
    while(_run && !(pm->get_show_fixed_page()) && pm->get_usb_reading() )
    {
        now = get_brightness(out__filter);
        if (now >= 255)
        {
            qDebug() << "已经触发拍照:" << now;
            break;
        }
        else if(now == -1)
        {
            Log::e(TAG,"usb已经拔出");
            goto end;
        }
        count += 10;
        if(count >  1000 * 30)// 60*60*24)
        {
            // _run  = false;
            Log::e(TAG,"usb超时30秒还不触发拍照事件,取消拍照");
            emit timeout();// error(tr("usb超时30秒还不触发拍照事件,取消拍照"));
            goto end;
        }
        QThread::msleep(10);
    }
    if(_run == false || pm->get_usb_reading() == false || pm->get_show_fixed_page() == true)
        goto end;

    ctrl->Run();
    count = 0;
    qDebug() << "已经拍照，等待出图"<< "-" << time.restart();
    while(_run)
    {
        QThread::msleep(10);
        count += 10;
        if(count > 5 *1000)
        {
            //        _run = false;
            Log::e(TAG,"触发拍照后, 5s也没收到图片, 取消拍照");
            goto end;
            //emit error(tr("触发拍照后, 5s也没收到图片, 取消拍照"));
            break;
        }

        if(_got_picture == true)
        {
            Log::i(TAG,"已经获取到图片,即将退出采集线程");
            break;
        }
    }



end:
    if(out__filter)
        ;//  qDebug() << "重置usb:" << set_brightness(out__filter, 0);
    if (ctrl)
    {

        qDebug() << "释放资源前"<< "+" << time.restart();
        ctrl->StopWhenReady();
        qDebug() << "释放资源1"<< "+" << time.restart();
        ctrl->Release();
        qDebug() << "释放资源2"<< "+" << time.restart();
    }
    if(pGraph)
    {
        pGraph->Release();
        qDebug() << "释放资源3"<< "+" << time.restart();
    }
    if(pBuild)
    {
        pBuild->Release();
        qDebug() << "释放资源4"<< "+" << time.restart();
    }
    CoUninitialize();
    qDebug() << "释放资源5"<< "+" << time.restart();
    return is_connected;
}
bool UsbReader::get_version_and_ser_num()
{
    auto pm = PatientManager::get_instance()->get_picture_manager();

    HRESULT ret = CoInitialize(NULL);
    bool is_connected = false;
    if (FAILED(ret))
    {
        //_run = false;
        emit error(tr("失败1"));
        goto  end;
    }

    IGraphBuilder* pGraph = nullptr;
    ICaptureGraphBuilder2* pBuild = nullptr;
    //创建用于抓拍的Sample Grabber Filter.
    IBaseFilter* pGrabberStill = NULL;
    ISampleGrabber* pSampleGrabberStill = NULL;
    IBaseFilter* pNull = nullptr;
    IMediaControl* ctrl = nullptr; //控制
    IBaseFilter* out__filter = nullptr;
    IMoniker* selected_imoniker = nullptr;
    bool ok = InitCaptureGraphBuilder(&pGraph, &pBuild);
    if (!ok)
    {
        Log::e(TAG, "失败2");
        emit error(tr("失败2"));
        goto end;
    }

    ok = init_yuv_receiver(&pGrabberStill, &pSampleGrabberStill);
    if (!ok)
    {
        Log::e(TAG, "失败3");
        emit error(tr("失败3"));
        goto end;
    }

    ret = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pNull);
    if (FAILED(ret))
    {
        Log::e(TAG,"创建隐藏窗口过滤器失败4");
        emit error(tr("创建隐藏窗口过滤器失败4"));
        goto end;
    }
    ret = pGraph->QueryInterface(IID_IMediaControl, (LPVOID*)&ctrl);
    if (FAILED(ret))
    {
        Log::e(TAG, "失败5");
        emit error(tr("失败5"));
        goto end;
    }

    ok = get_input_filter(std::string("RGBIC UVC LION"), &out__filter, &selected_imoniker); //MKL Camera ,RGBIC UVC LION
    if (!ok || out__filter == nullptr)
    {
        Log::e(TAG,"失败6");
        emit error(tr("失败6"));
        goto end;
    }

    emit connected();
    is_connected = true;

    //开始构建预览graph
    //加入设备抓拍filter
    ret = pGraph->AddFilter(out__filter, L"Video Capture");
    if (FAILED(ret))
    {
        out__filter->Release();
        out__filter = nullptr;
        Log::e(TAG,"失败7");
        emit error(tr("失败7"));
        goto end;
    }

    //加入DirectShow中自带的SampleGrabber Filter
    ret = pGraph->AddFilter(pGrabberStill, L"Still Sample Grabber");
    if (FAILED(ret))
    {
        Log::e(TAG,"失败8");
        pGrabberStill->Release();
        pGrabberStill = nullptr;
        emit error(tr("失败8"));
        goto end;
    }

    //加入DirectShow中自带的NullRender Filter
    ret = pGraph->AddFilter(pNull, L"NullRender");
    if (FAILED(ret))
    {
        Log::e(TAG,"失败9");
        pNull->Release();
        pNull = nullptr;
        emit error(tr("失败9"));
        goto end;
    }
    /*
    //加入DirectShow中自带的SampleGrabber Filter
    hr = pGraph->AddFilter(pGrabberF, L"Sample Grabber");
    if (FAILED(hr))
    {
        printf("Couldn't add sample grabber to graph!  hr=0x%x\n", hr);
        // Return an error.
    }
    */

    //使用Capture Graph Builder构建预览 Graph

    ret = pBuild->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, out__filter, pGrabberStill, pNull);
    if (FAILED(ret))
    {
        Log::e(TAG,"失败10");
        out__filter->Release();
        out__filter = nullptr;
        pGrabberStill->Release();
        pGrabberStill = nullptr;
        pNull->Release();
        pNull = nullptr;
        emit error(tr("失败10"));
        goto end;
    }

    //configure the Sample Grabber so that it buffers samples :
    ret = pSampleGrabberStill->SetOneShot(true);
    ret = pSampleGrabberStill->SetBufferSamples(false);


    //获取设备输出格式信息
    AM_MEDIA_TYPE mt;
    ret = pSampleGrabberStill->GetConnectedMediaType(&mt);
    if (FAILED(ret))
    {
        Log::e(TAG,"失败11");
        emit error(tr("失败11"));
        goto end;
    }

    qDebug() << (mt.subtype == MEDIASUBTYPE_NV12) << "," << (mt.subtype == MEDIASUBTYPE_YV12) << "," << (mt.subtype == MEDIASUBTYPE_YUY2);
    VIDEOINFOHEADER* vih = (VIDEOINFOHEADER*)mt.pbFormat;
    qDebug() << "width" << vih->bmiHeader.biWidth << "height" << vih->bmiHeader.biHeight << Qt::endl;
    CSampleGrabberCB* CB = new CSampleGrabberCB();

    CB->set_data_listener([this](BYTE * buf, long size){
        //TODO 数据格式为YUY2，需要转换
        int len = 1660 * 2280* 2;
        qDebug() << "样本" << size << "所需" << len;
        // char FileName[256] = "data.yuv";
        //	sprintf_s(FileName, "data.yuv");
        // FILE * out = fopen(FileName, "wb");
        if(size < len)
        {
            Log::e(TAG,QString("usb 数据长度不对:%1").arg(size));
            emit error(QString(tr("usb 数据长度不对:")+ "%1").arg(size));
            _got_picture = true;
            emit got_picture(nullptr);
            return;
        }
        QByteArray * data = new QByteArray();//(const char *)buf, len);
        *data = QByteArray((const char *)buf, len);
        emit got_picture(data);
        _got_picture = true;
        //fwrite(pBuffer, 1, len, out);
        //   fclose(out);
    });

    //设置触发抓拍后，调用的回调函数 0-调用SampleCB 1-BufferCB
    ret = pSampleGrabberStill->SetCallback(CB, 1);
    pm->set_usb_version( get_version(out__filter));
    pm->set_usb_ser_num(get_ser_number(out__filter));

end:
    if(out__filter)
        qDebug() << "重置usb:" << set_brightness(out__filter, 0);
    if (ctrl)
    {
        ctrl->StopWhenReady();
        ctrl->Release();
    }
    if(pGraph)
        pGraph->Release();
    if(pBuild)
        pBuild->Release();
    CoUninitialize();

    return is_connected;
}

bool UsbReader::run_fixed()
{
    QTime time;
    time.start();
    HRESULT ret = CoInitialize(NULL);
    bool is_connected = false;
    if (FAILED(ret))
    {
        //_run = false;
        emit error(tr("失败1"));
        goto  end;
    }

    IGraphBuilder* pGraph = nullptr;
    ICaptureGraphBuilder2* pBuild = nullptr;
    //创建用于抓拍的Sample Grabber Filter.
    IBaseFilter* pGrabberStill = NULL;
    ISampleGrabber* pSampleGrabberStill = NULL;
    IBaseFilter* pNull = nullptr;
    IMediaControl* ctrl = nullptr; //控制
    IBaseFilter* out__filter = nullptr;
    IMoniker* selected_imoniker = nullptr;
    bool ok = InitCaptureGraphBuilder(&pGraph, &pBuild);
    if (!ok)
    {
        Log::e(TAG, "失败2");
        emit error(tr("失败2"));
        goto end;
    }
    else
        qDebug() << "ok-1"<< "-" << time.restart();

    ok = init_yuv_receiver(&pGrabberStill, &pSampleGrabberStill);
    if (!ok)
    {
        Log::e(TAG, "失败3");
        emit error(tr("失败3"));
        goto end;
    }    else
        qDebug() << "ok-2"<< "-" << time.restart();

    ret = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pNull);
    if (FAILED(ret))
    {
        Log::e(TAG,"创建隐藏窗口过滤器失败4");
        emit error(tr("创建隐藏窗口过滤器失败4"));
        goto end;
    }
    else
        qDebug() << "ok-3"<< "-" << time.restart();
    ret = pGraph->QueryInterface(IID_IMediaControl, (LPVOID*)&ctrl);
    if (FAILED(ret))
    {
        Log::e(TAG, "失败5");
        emit error(tr("失败5"));
        goto end;
    }
    else
        qDebug() << "ok-4"<< "-" << time.restart();

    ok = get_input_filter(std::string("RGBIC UVC LION"), &out__filter, &selected_imoniker); //MKL Camera ,RGBIC UVC LION
    if (!ok || out__filter == nullptr)
    {
        Log::e(TAG,"失败6");
        emit error(tr("失败6"));
        goto end;
    }
    else
        qDebug() << "ok-5"<< "-" << time.restart();

    emit connected();
    is_connected = true;

    //开始构建预览graph
    //加入设备抓拍filter
    ret = pGraph->AddFilter(out__filter, L"Video Capture");
    if (FAILED(ret))
    {
        out__filter->Release();
        out__filter = nullptr;
        Log::e(TAG,"失败7");
        emit error(tr("失败7"));
        goto end;
    }
    else
        qDebug() << "ok-6"<< "-" << time.restart();

    //加入DirectShow中自带的SampleGrabber Filter
    ret = pGraph->AddFilter(pGrabberStill, L"Still Sample Grabber");
    if (FAILED(ret))
    {
        Log::e(TAG,"失败8");
        pGrabberStill->Release();
        pGrabberStill = nullptr;
        emit error(tr("失败8"));
        goto end;
    }    else
        qDebug() << "ok-7"<< "-" << time.restart();

    //加入DirectShow中自带的NullRender Filter
    ret = pGraph->AddFilter(pNull, L"NullRender");
    if (FAILED(ret))
    {
        Log::e(TAG,"失败9");
        pNull->Release();
        pNull = nullptr;
        emit error(tr("失败9"));
        goto end;
    }
    else
        qDebug() << "ok-8"<< "-" << time.restart();
    /*
    //加入DirectShow中自带的SampleGrabber Filter
    hr = pGraph->AddFilter(pGrabberF, L"Sample Grabber");
    if (FAILED(hr))
    {
        printf("Couldn't add sample grabber to graph!  hr=0x%x\n", hr);
        // Return an error.
    }
    */

    //使用Capture Graph Builder构建预览 Graph

    ret = pBuild->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, out__filter, pGrabberStill, pNull);
    if (FAILED(ret))
    {
        Log::e(TAG,"失败10");
        out__filter->Release();
        out__filter = nullptr;
        pGrabberStill->Release();
        pGrabberStill = nullptr;
        pNull->Release();
        pNull = nullptr;
        emit error(tr("失败10"));
        goto end;
    }
    else
        qDebug() << "ok-9"<< "-" << time.restart();

    //configure the Sample Grabber so that it buffers samples :
    ret = pSampleGrabberStill->SetOneShot(true);
    ret = pSampleGrabberStill->SetBufferSamples(false);


    //获取设备输出格式信息
    AM_MEDIA_TYPE mt;
    ret = pSampleGrabberStill->GetConnectedMediaType(&mt);
    if (FAILED(ret))
    {
        Log::e(TAG,"失败11");
        emit error(tr("失败11"));
        goto end;
    }    else
        qDebug() << "ok-10"<< "-" << time.restart();

    qDebug() << (mt.subtype == MEDIASUBTYPE_NV12) << "," << (mt.subtype == MEDIASUBTYPE_YV12) << "," << (mt.subtype == MEDIASUBTYPE_YUY2);
    VIDEOINFOHEADER* vih = (VIDEOINFOHEADER*)mt.pbFormat;
    qDebug() << "width" << vih->bmiHeader.biWidth << "height" << vih->bmiHeader.biHeight << Qt::endl;
    CSampleGrabberCB* CB = new CSampleGrabberCB();

    CB->set_data_listener([this](BYTE * buf, long size){
        //TODO 数据格式为YUY2，需要转换
        int len = 1660 * 2280* 2;
        qDebug() << "样本" << size << "所需" << len;
        // char FileName[256] = "data.yuv";
        //	sprintf_s(FileName, "data.yuv");
        // FILE * out = fopen(FileName, "wb");
        if(size < len)
        {
            Log::e(TAG,QString("usb 数据长度不对:%1").arg(size));
            emit error(QString(tr("usb 数据长度不对:")+ "%1").arg(size));
            _got_picture = true;
            emit got_picture(nullptr);
            return;
        }
        // QByteArray * data = new QByteArray();//(const char *)buf, len);
        this->buf = QByteArray((const char *)buf, len);//不存为图片,
        // emit got_picture(data);
        _got_picture = true;
        //fwrite(pBuffer, 1, len, out);
        //   fclose(out);
    });

    //设置触发抓拍后，调用的回调函数 0-调用SampleCB 1-BufferCB
    ret = pSampleGrabberStill->SetCallback(CB, 1);
    long now = get_brightness(out__filter);
    if (now != 0)
    {
        qDebug() << "重置usb:" << set_brightness(out__filter, 0);
        QThread::msleep(100);
    }

    auto pm = PatientManager::get_instance()->get_picture_manager();
    pm->set_usb_version( get_version(out__filter));
    pm->set_usb_ser_num(get_ser_number(out__filter));

    qDebug() << "set_brightness" << set_brightness(out__filter, _cmd);
    QThread::msleep(100);
    qDebug() << "get_brightness" << get_brightness(out__filter);
    int count = 0;

    qDebug() << "等待拍照"<< "-" << time.restart();
    _got_picture = false;
    while(pm->get_show_fixed_page() && pm->get_fixed_mode_starting())
    {
        now = get_brightness(out__filter);
        if (now >= 255)
        {
            qDebug() << "已经触发拍照:" << now;
            break;
        }
        count += 10;
        if(count >  1000 * 120)
        {
            // _run  = false;
            Log::e(TAG,"usb超时2分钟还不触发拍照事件,取消拍照");
            goto end;
            //  emit error(tr("usb超时5分钟还不触发拍照事件,取消拍照"));
            break;
        }
        QThread::msleep(10);
    }
    if(!pm->get_show_fixed_page() || !pm->get_fixed_mode_starting()) //
        goto end;
    ctrl->Run();
    count = 0;
    qDebug() << "已经拍照，等待出图"<< "-" << time.restart();
    while(pm->get_show_fixed_page() && pm->get_fixed_mode_starting())
    {
        QThread::msleep(10);
        count += 10;
        if(count > 5 *1000)
        {
            //          _run = false;
            Log::e(TAG,"触发拍照后, 5s也没收到图片, 取消拍照");
            emit error(tr("触发拍照后, 5s也没收到图片, 取消拍照"));
            goto end;


        }

        if(_got_picture == true)
        {
            Log::i(TAG,"已经获取到图片,即将退出采集线程");
            break;
        }
    }



end:
    if(out__filter)
        ;// qDebug() << "重置usb:" << set_brightness(out__filter, 0);
    if (ctrl)
    {
        qDebug() << "释放资源前"<< "-" << time.restart();
        ctrl->StopWhenReady();
        qDebug() << "释放资源1"<< "-" << time.restart();
        ctrl->Release();
        qDebug() << "释放资源2"<< "-" << time.restart();
    }
    if(pGraph)
    {
        pGraph->Release();
        qDebug() << "释放资源3"<< "-" << time.restart();
    }
    if(pBuild)
    {
        pBuild->Release();
        qDebug() << "释放资源4"<< "-" << time.restart();
    }
    CoUninitialize();
    qDebug() << "释放资源5"<< "-" << time.restart();
    return is_connected;
}

int UsbReader::check_fix_dark_pic_is_ok(double & av)
{
    int w =1660,h=2280;
    QByteArray & data  = buf;

    quint16 * ptr = (quint16*)data.data();
    double sum_ct_value = 0;
    for(int i =0; i < w*h;i++)
    {
        sum_ct_value += ptr[i];
    }
    av = sum_ct_value /double(w*h); //平均ct
    if(av < 64)
        return -1;
    else if(av > 180)
        return 1;
    else
        return 0;
    //    bool ok =  average < 180;//32

    //    return ok ? 0 : 1;//0合适，1偏大
}

int UsbReader::check_fix_light_pic_is_ok(double & av)
{
    int w =1660,h=2280;
    QByteArray & data  = buf;

    quint16 * ptr = (quint16*)data.data();
    double sum_ct_value = 0;
    for(int i =0; i < w*h;i++)
    {
        sum_ct_value += ptr[i];
    }
    av = sum_ct_value /double(w*h); //平均ct
    if(4095 * 0.4  > av)
        return -1;
    else if(av > 4095 * 0.75)
        return 1;
    else
        return 0;

}

bool UsbReader::init_yuv_receiver(IBaseFilter** ppGrabberStill, ISampleGrabber** ppSampleGrabberStill)
{
    IBaseFilter* pGrabberStill = NULL;
    ISampleGrabber* pSampleGrabberStill = NULL;
    //创建用于抓拍的Sample Grabber Filter.
    auto hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pGrabberStill);
    if (FAILED(hr))
        return false;

    //获取ISampleGrabber接口，用于设置回调等相关信息
    hr = pGrabberStill->QueryInterface(IID_ISampleGrabber, (void**)&pSampleGrabberStill);
    if (FAILED(hr))
    {
        pGrabberStill->Release();
        return false;
    }
    *ppGrabberStill = pGrabberStill;
    *ppSampleGrabberStill = pSampleGrabberStill;
    return true;
}

bool UsbReader::InitCaptureGraphBuilder(IGraphBuilder** ppGraph,  ICaptureGraphBuilder2** ppBuild  )
{
    if (!ppGraph || !ppBuild)
    {
        qDebug() <<"失败11";
        return false;
    }
    IGraphBuilder* pGraph = NULL;
    ICaptureGraphBuilder2* pBuild = NULL;

    // Create the Capture Graph Builder.
    HRESULT hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL,
                                  CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&pBuild);
    if (SUCCEEDED(hr))
    {
        // Create the Filter Graph Manager.
        hr = CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER,
                              IID_IGraphBuilder, (void**)&pGraph);
        if (SUCCEEDED(hr))
        {
            // Initialize the Capture Graph Builder.
            pBuild->SetFiltergraph(pGraph);

            // Return both interface pointers to the caller.
            *ppBuild = pBuild;
            *ppGraph = pGraph; // The caller must release both interfaces.
            qDebug()<<"成功22";
            return true;
        }
        else
        {
            pBuild->Release();
        }
    }
    qDebug() << "失败33";
    return false;
}


long UsbReader::get_brightness(IBaseFilter* bf)
{

    IAMVideoProcAmp* pProcAmp = 0;
    long ret = -1;
    HRESULT hr = bf->QueryInterface(IID_IAMVideoProcAmp, (void**)&pProcAmp);
    if (SUCCEEDED(hr))
    {
        long Flags, Val;
        // 亮度.
        if (SUCCEEDED(hr))
        {
            hr = pProcAmp->Get(VideoProcAmp_Brightness, &Val, &Flags);
            if (SUCCEEDED(hr))
                ret = Val;
        }
        pProcAmp->Release();
    }
    return ret;
}

bool UsbReader::set_brightness(IBaseFilter* bf,long value)
{
    IAMVideoProcAmp* pProcAmp = 0;
    bool ret = false;
    HRESULT hr = bf->QueryInterface(IID_IAMVideoProcAmp, (void**)&pProcAmp);
    if (SUCCEEDED(hr))
    {
        long Flags, Val;
        // 亮度.
        if (SUCCEEDED(hr))
        {
            hr = pProcAmp->Get(VideoProcAmp_Brightness, &Val, &Flags);
            hr = pProcAmp->Set(VideoProcAmp_Brightness, value, Flags);
            if (SUCCEEDED(hr))
                ret = true;
        }
        pProcAmp->Release();
    }
    return ret;
}

QString UsbReader::get_version(IBaseFilter *bf)
{
    IAMVideoProcAmp* pProcAmp = 0;
    QString ret;
    HRESULT hr = bf->QueryInterface(IID_IAMVideoProcAmp, (void**)&pProcAmp);
    if (SUCCEEDED(hr))
    {
        long Flags, Val;
        quint8 *  v = ( quint8  *)(&Val);
        // 亮度. 00000001
        if (SUCCEEDED(hr))
        {
            hr = pProcAmp->Get(VideoProcAmp_Contrast, &Val, &Flags);
            quint8 a =1;
            a = (a << 6);
            Val = 0;
            v[1]=a;
            hr = pProcAmp->Set(VideoProcAmp_Contrast, Val, Flags);
            hr = pProcAmp->Get(VideoProcAmp_Contrast, &Val, &Flags);

            QString temp("%1.%2");
            temp = temp.arg(v[1]).arg(v[0]);
            qDebug() << v[0] << v[1];
            ret = temp;
            //  hr = pProcAmp->Get(VideoProcAmp_Contrast, &Val, &Flags);


        }
        pProcAmp->Release();
    }
    return ret;
}
QString reversalStr(QString &str)
{
    QString ret;
    for(QChar sz:str)
    {
        ret.push_front(sz);
    }
    return ret;
}

QString UsbReader::get_ser_number(IBaseFilter *bf)
{
    IAMVideoProcAmp* pProcAmp = 0;
    //long ret = 0;
    char ret[64];
    memset(ret,0,64);

    HRESULT hr = bf->QueryInterface(IID_IAMVideoProcAmp, (void**)&pProcAmp);
    if (SUCCEEDED(hr))
    {
        long Flags, Val;
        hr = pProcAmp->Get(VideoProcAmp_Contrast, &Val, &Flags);
        // 亮度.
        quint8 *  v = ( quint8  *)(&Val);

        quint32 factors[16];
        factors[0]=1;
        for(int i =1;i<16;i++)
        {
            factors[i] =factors[i-1]*2;
        }


        if (SUCCEEDED(hr))
        {
            //  QString left = QString("00000000");
            // QString right = QString("00");

            //  QString cmd = +"000000"+"01";
            //第三个是int QString::toInt(bool *ok = nullptr, int base = 10) const
            // hr = pProcAmp->Get(VideoProcAmp_Contrast, &Val, &Flags);
            for(int i =0; i < 64;i++)
            {
                //   quint8 index = i;
                // index = index << 2;

                //                QString temp  =QString("%1").arg(index , 0, 2) ;
                //                while(temp.size() < 6)
                //                    temp  = "0"+temp;
                //                 temp = reversalStr(temp);
                //                temp = temp + right;
                //qDebug() << temp;


                //                QString left ="00"+temp;

                //                temp = left+temp+right;
                //                qDebug() <<temp ;//<< reversalStr(temp);
                //               // temp = reversalStr(temp);
                //                //temp.reverse();
                //              quint8  result = 0;
                //                for(int i = 0;i < 8;i++)
                //                {
                //                    if(temp.at(i) == "1")
                //                    {
                //                        result += factors[i];
                //                    }
                //                }
                // long  cmd = temp.toLong(nullptr, 2);  //swapInt32(cmd);
                //  result = swapInt32(result);


                Val = 0;
                v[1] = i;
                hr = pProcAmp->Set(VideoProcAmp_Contrast, Val, Flags);
                hr = pProcAmp->Get(VideoProcAmp_Contrast, &Val, &Flags);
                //   qDebug() <<  Val << "--------------" << v[0] << v[1] << v[2] << v[3] << i;
                ret[i] = v[0];
                if(v[0] ==0)
                {
                    break;
                }
            }// v[0]是对应的数字
        }
        pProcAmp->Release();
    }
    return ret;
}

bool UsbReader::get_input_filter(const std::string & device, IBaseFilter** out__filter, IMoniker ** selected_imoniker)//
{
    HRESULT hr;
    ULONG cFetched;

    //https://docs.microsoft.com/zh-cn/windows/desktop/DirectShow/selecting-a-capture-device
    //遍历设备
    ICreateDevEnum* pCreateDevEnum;
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pCreateDevEnum);
    if (FAILED(hr))
    {
        qDebug() <<"Failed to enumerate all video and audio capture devices!";
        return false;
    }

    IEnumMoniker* pEnumMoniker;
    hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumMoniker, 0);
    if (FAILED(hr) || !pEnumMoniker)
    {
        qDebug() << "Failed to create ClassEnumerator!";
        return false;
    }

    IMoniker* pMoniker = NULL;
    IBaseFilter* ret = NULL;
    while (pEnumMoniker ->Next(1, &pMoniker, NULL) == S_OK)
    {
        IPropertyBag* pPropBag;
        HRESULT hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
        if (FAILED(hr))
        {
            pMoniker->Release();
            continue;
        }

        VARIANT var;
        VariantInit(&var);

        // Get description or friendly name.
        hr = pPropBag->Read(L"Description", &var, 0);
        if (FAILED(hr))
        {
            hr = pPropBag->Read(L"FriendlyName", &var, 0);
        }

        if (SUCCEEDED(hr))
        {
            qDebug() <<  var.bstrVal;
            std::string name = QString::fromStdWString(std::wstring(var.bstrVal)).toStdString();
            if (name == device)
            {
                qDebug() << "found device ";
                hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&ret);
                if (FAILED(hr))
                {
                    qDebug() << "Couldn't bind moniker to filter object!";
                    pPropBag->Release();
                    pMoniker->Release();
                }
                else
                {
                    pPropBag->Release();
                    *out__filter = ret;
                    *selected_imoniker = pMoniker; //需要持有它
                }
                break;
            }
            else
            {
                pPropBag->Release();
                pMoniker->Release();
                continue;
            }
        }
        else
        {
            pPropBag->Release();
            pMoniker->Release();
            continue;
        }
    }
    return true;
}

STDMETHODIMP_(ULONG) UsbReader::CSampleGrabberCB::AddRef()
{
    return 2;
}

STDMETHODIMP_(ULONG) UsbReader::CSampleGrabberCB::Release()
{
    return 1;
}

UsbReader::CSampleGrabberCB::CSampleGrabberCB()
{

}

HRESULT UsbReader::CSampleGrabberCB::QueryInterface(const IID &riid, void **ppv)
{

    //CheckPointer(ppv,E_POINTER);

    if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown )
    {
        *ppv = (void *) static_cast<ISampleGrabberCB*> ( this );
        return NOERROR;
    }

    return E_NOINTERFACE;


}

HRESULT UsbReader::CSampleGrabberCB::SampleCB(double SampleTime, IMediaSample *pSample)
{

    qDebug() << "样板11";
    return 0;

}

HRESULT UsbReader::CSampleGrabberCB::BufferCB(double SampleTime, BYTE *pBuffer, long BufferSize)
{
    if(_data_listener != nullptr)
    {
        _data_listener(pBuffer,BufferSize);
        _data_listener = nullptr;
    }
    return 0;

}

void UsbReader::CSampleGrabberCB::set_data_listener(std::function<void (BYTE *, long)> data_listener)
{
    this->_data_listener = data_listener;
}
