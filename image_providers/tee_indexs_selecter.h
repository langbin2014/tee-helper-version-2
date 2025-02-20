#ifndef TEE_INDEXS_SELECTER_H
#define TEE_INDEXS_SELECTER_H
#include <QJsonArray>
#include <QQuickImageProvider>
#include <QMutex>
class TeeIndexsSelecterImageProvider:public QQuickImageProvider
{
    const bool DEBUG = false;
    const QString  TAG = "牙齿选择图片类";
public:
    TeeIndexsSelecterImageProvider();
    virtual QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
    QString get_url();
    void select(const QString & tee_type);
    void reinit(const QString & tee_type,const QString & tee_indexs);
  //  Q_INVOKABLE void click(float x, float y,);
 //  QString collecting_collecting_tee_type() const;
//    void setCollecting_collecting_tee_type(const QString &collecting_collecting_tee_type);

 //   QJsonArray collecting_tee_indexs() const;
//    void setCollecting_tee_indexs(const QJsonArray &collecting_tee_indexs);

    void mouse_event(float x, float y, const QString & action);//action是鼠标动作("m","c"), doing是为了哪个过滤做选择，可能的参数是 collecting,和 collected
    bool dirty();
public:
    void init();
    void get_database_indexs_format(QJsonArray &out_indexs);
private:
    void init(QImage* pic,int y1,int y2,int y3,int y4,std::vector<QRect> & out);

private:
//    QString _collecting_collecting_tee_type; //收集中的牙齿类型
//    QJsonArray _collecting_tee_indexs; //收集中牙齿编号
    bool _dirty = false;
    std::vector<QRect> _adult_rects;
    std::vector<QRect> _baby_rects;
    std::vector<QRect> * _current_rect;

    std::vector<bool> _adult_checkeds;
    std::vector<bool> _baby_checkeds;
    std::vector<bool> * _current_checkeds;
    QImage * _adult;
    QImage * _baby;
    QImage * _current_selected_iamge;

    QImage * _adult_table;
    QImage * _baby_table;
    QImage * _current_table;

    std::vector<QImage *>  _adult_unchecked_images;
    std::vector<QImage *>  _adult_checked_images;
    std::vector<QImage *>  _baby_unchecked_images;
    std::vector<QImage *>  _baby_checked_images;
    std::vector<QImage *> * _current_unchecked_images;
    std::vector<QImage *> * _current_checked_images;

    QMutex mutex;

};

#endif // TEEINDEXSSELECTER_H
