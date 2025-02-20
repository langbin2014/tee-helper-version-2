#ifndef USB_READER_H
#define USB_READER_H

#include<QThread>
#include<QString>
#include"qedit.h"
class IGraphBuilder;
class ICaptureGraphBuilder2;
class IBaseFilter;
class IMoniker;
class UsbReader :public QThread
{
    static  QString TAG;
    static  bool DEBUG_;
    Q_OBJECT
public:
    enum class Mode1{
        AV ,
        //设置亮度命令后，FPGA 将会在 30ms 左右自动触发。回读亮度命令，
        //判断亮度命令 Bit8 为 1，上位机就可以开始获取图像桢

        VTC,
        //        设置亮度命令后，FPGA 将会等待传感器上的 X-ray 照射结束才自动触
        //发。外部 X-ray 设备对传感器进行照射，可能在数分钟后才执行，并持续 0.01-1 秒。所以
        //PC 上位机需要持续读取亮度命令的 Bit8 触发标准位，等待该标志位 1 后，上位机才开始获
        //取图像桢。
    };

    enum class Bin2{
        NoBining,Binnig
    };

    enum class Filter3{
        NoFilter,
        FilterForDropBadPoint //丢弃坏点
    };
    class CSampleGrabberCB : public ISampleGrabberCB
    {
    public:
        STDMETHODIMP_(ULONG) AddRef();
        STDMETHODIMP_(ULONG) Release();
        CSampleGrabberCB();
        // Fake out any COM QI'ing
        STDMETHODIMP QueryInterface(REFIID riid, void ** ppv);
        STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample );
        STDMETHODIMP BufferCB( double SampleTime, BYTE * pBuffer, long BufferSize );
        void set_data_listener(std::function<void (BYTE * buf, long size)> data_listener);
    private:
        std::function<void (BYTE * buf, long size)> _data_listener = nullptr;
    };

    UsbReader(QObject * parent);
    void stop();
    void begin_cap(UsbReader::Mode1 mode, UsbReader::Bin2 bin, UsbReader::Filter3 filter);
    void get_usb_info();
signals:
    void error(const QString & error);
    void connected();
    void got_picture(QByteArray * data);
    void timeout();
protected:
    virtual void run() override;
    bool run_2();
    bool run_fixed();
    int check_fix_dark_pic_is_ok(double & av);
    int check_fix_light_pic_is_ok(double & av);
private:
    static bool check_fixed_file_is_ok();
    static  bool InitCaptureGraphBuilder(IGraphBuilder** ppGraph, ICaptureGraphBuilder2** ppBuild);
    static  bool init_yuv_receiver(IBaseFilter** ppGrabberStill, ISampleGrabber** ppSampleGrabberStill);
    static bool set_brightness(IBaseFilter* bf,long value);
    static QString get_version(IBaseFilter* bf);
    QString get_ser_number(IBaseFilter* bf);
    static  long get_brightness(IBaseFilter* bf);
    static bool get_input_filter(const std::string & device, IBaseFilter** out__filter, IMoniker ** selected_imoniker);
    static int get_mode_setting(Mode1 mode,Bin2 bin,Filter3 filter);
    bool get_version_and_ser_num();
private:

    bool _run;

    long _cmd;
    bool _got_picture;
    QByteArray  buf;;
    std::function<void()> task = nullptr;
};

#endif // USB_READER_H
