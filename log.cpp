#include<QFile>
#include<QMutex>
#include<QTextStream>
#include<memory>
#include"log.h"

static QMutex mutex;
static QString path = "opengl.log";
static std::shared_ptr<QFile> file(0);
static std::shared_ptr< QTextStream> ts(0);

void saveToFile(const QString& tag, const QString& type, const QString& msg)
{
    QMutexLocker l(&mutex);
    if(file.get() == nullptr)
    {
        file = std::shared_ptr<QFile>(new QFile(path));
        file->open(QIODevice::WriteOnly);//| QIODevice::Append
        ts = std::shared_ptr<QTextStream>(new QTextStream(file.get()));
    }
    *ts << tag << type << msg <<endl;
}

void Log::d(const QString& tag, const QString& msg)
{
    qDebug() << tag << "DEBUG:" << msg;
    saveToFile(tag, "DEBUG:", msg);
}

void Log::d(const QString & tag,const std::string & msg)
{
    qDebug() << tag << "DEBUG: "<< msg.c_str();
    saveToFile(tag, "DEBUG: ", QString::fromStdString(msg));
}

void Log::d(const QString & tag,const char *  msg)
{
    qDebug() << tag << "DEBUG: "<< msg;
    saveToFile(tag, "DEBUG: ", msg);
}

void Log::i(const QString & tag,const QString & msg)
{
    qInfo() << tag << "INFO: "<< msg;
    saveToFile(tag, "INFO: ", msg);
}

void Log::i(const QString & tag,const std::string & msg)
{
    qInfo() << tag << "INFO: "<< msg.c_str();
    saveToFile(tag, "INFO: ", QString::fromStdString(msg));
}

void Log::i(const QString & tag,const char *  msg)
{
    qInfo() << tag << "INFO: "<< msg;
    saveToFile(tag, "INFO: ", msg);
}

void Log::e(const QString & tag,const QString & msg)
{
    qWarning() << tag << "ERROR: "<< msg;
    saveToFile(tag, "ERROR: ", msg);
}
