#ifndef EIDTING_PAGE_PICTURE_MODEL_H
#define EIDTING_PAGE_PICTURE_MODEL_H

#include <QJsonArray>
#include <QJsonObject>
#include <QJSValue>
#include <QAbstractListModel>
#include"c++/table/images_table.h"
/**
 * @brief The EditingPictureModel class
 * 选中出现在中间，编辑中的model
 */
class EditingPictureModel : public QAbstractListModel
{
    const QString TAG = "编辑中的图片model类";
    Q_OBJECT
    Q_PROPERTY(int count      READ getCount   WRITE setCount    NOTIFY countChanged);
public:

    EditingPictureModel(QObject * parent = nullptr);
    void reset_to_empty(int eles_count); //



    void checked_export_or_delete(bool checked);
    void append(const QJsonObject &info);
    void remove(const QString & picture_id);
    void delete_checked_pictures();//
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    bool update_property(const QString &picture_id, const QJsonObject &new_propertys);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role ) override;
//    void select(ImagesTable::PictureSoureType picture_soure_type, ImagesTable::PictureLevel picture_level, ImagesTable::PictureHasTeeType picture_has_tee_type, qint64 begin, qint64 end, const QJsonArray & tee_indexs, const QString &tee_type, const QList<QJsonObject> &all_data);
//    bool test_filter(const QJsonObject & picture, ImagesTable::PictureSoureType picture_soure_type,ImagesTable::PictureLevel picture_level,ImagesTable::PictureHasTeeType picture_has_tee_type,qint64 begin,qint64 end,const QJsonArray & tee_indexs);


    int getCount() const;
    void setCount(int count);
    void get_checked_files(QStringList & out) const;
    int get_pics() const;
    QStringList get_checked_ids();
signals:
    void countChanged();
protected:
    virtual QHash<int, QByteArray> roleNames() const override;
private:
      void clear();
    QList<QJsonObject> _selected_data;
    QHash<int, QByteArray> roles;
    qint64 _today_pts;
    int _count = 0;
    QJsonObject null_info;
};
#endif // EIDTINGPAGEEditingPictureModel_H
