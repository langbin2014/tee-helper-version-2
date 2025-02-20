#ifndef TODAY_PIC_MODEL_H
#define TODAY_PIC_MODEL_H

#include <QJsonArray>
#include <QJsonObject>
#include <QJSValue>
#include <QAbstractListModel>
#include"c++/table/images_table.h"
class Today_Pic_Model : public QAbstractListModel
{
    const QString TAG = "图片mo3del类";
    Q_OBJECT
       Q_PROPERTY(int count      READ getCount   WRITE setCount    NOTIFY countChanged);
public:
    Today_Pic_Model(QObject * parent = nullptr);
    void clear();
    void set_all_checked_for_on_editing(bool checked);
    void set_one_checked_for_on_editing(const QString & picture_id, bool checked);
     void set_one_checked_for_on_editing(int index, bool checked);
    void checked_export_or_delete(bool checked);
     void append(const QJsonObject &info);
    void remove_first();
    void remove(const QString & picture_id);
   // void remove(QJsonObject * info);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    bool update_property(const QString & picture_id, const QJsonObject &new_propertys);
    void checked_thoes_ids(const QStringList &picture_id);
    //(const QJsonObject * ele,const QJsonObject &new_propertys);
   QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role ) override;
   // void select(ImagesTable::PictureSoureType picture_soure_type, ImagesTable::PictureLevel picture_level, ImagesTable::PictureHasTeeType picture_has_tee_type, qint64 begin, qint64 end, const QJsonArray & tee_indexs, const QString &tee_type, const QList<QJsonObject> &all_data);
  //  bool test_filter(const QJsonObject & picture, ImagesTable::PictureSoureType picture_soure_type,ImagesTable::PictureLevel picture_level,ImagesTable::PictureHasTeeType picture_has_tee_type,qint64 begin,qint64 end,const QJsonArray & tee_indexs);


    int getCount() const;
    void setCount(int count);
    void get_checked_files(QStringList & out) const;
    void delete_checked_pictures(std::map<QString, QString> &out);
    const QList<QJsonObject> &get_data() const;
signals:
    void countChanged();
    void checked_for_paint_list_changed(bool checked,const QString & picture_id);
protected:
    virtual QHash<int, QByteArray> roleNames() const override;
private:

    QList<QJsonObject> _selected_data;
     QHash<int, QByteArray> roles;
     qint64 _today_pts;
     int _count = 0;
     int _checked_for_paint_list_role;
     QString _picture_key;
};




#endif // TODAY_PIC_MODEL_H
