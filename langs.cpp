
#include "langs.h"
#include<QFile>
#include<QDebug>

Langs::Langs()
{
    langs[QString(files[0])]=(std::map<QString,QString>());
    it = langs.begin();
}

void Langs::set_index(int index)
{
    this->index = index;
    QString file=files[index];
    it =  langs.find(file);

    if(it == langs.end())
    {
        langs[file] = std::map<QString,QString>();
        it =  langs.find(file);

        QFile  f(file);
       qDebug() << "------------------------33----"  << f.open(QIODevice::ReadOnly |QIODevice::Text);
        while(!f.atEnd())
        {
            QByteArray data = f.readLine();
            QString str(data);
            str = str.replace("\"","");
            str = str.mid(0,str.size()-1);
            QStringList list = str.split("=");
            if(list.size() == 2)
            {
                (it->second)[list.at(0)] = list.at(1);
                qDebug() << list;
            }
        }

    }
}

void Langs::clear()
{
    langs.clear();
    langs[QString(files[0])]=(std::map<QString,QString>());
    it = langs.begin();
}

QString Langs::my_tr(const QString &key)
{
    auto kv = it->second.find(key);
    if(kv == it->second.end())
    {
        return key;
    }
    else
        return kv->second;
}

