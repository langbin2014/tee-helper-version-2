#ifndef LOG_H
#define LOG_H
#include<QDebug>
#include<sstream>
void saveToFile(const QString& tag, const QString& type, const QString& msg);
class Log : public QObject{
    Q_OBJECT
public:
    Q_INVOKABLE static void d(const QString& tag, const QString& msg);
    Q_INVOKABLE static void d(const QString & tag,const std::string & msg);
    Q_INVOKABLE static void d(const QString & tag,const char *  msg);
    Q_INVOKABLE static void i(const QString & tag,const QString & msg);
    Q_INVOKABLE static void i(const QString & tag,const std::string & msg);
    Q_INVOKABLE static void i(const QString & tag,const char *  msg);
    Q_INVOKABLE static void e(const QString & tag,const QString & msg);
};



#endif // LOG_H
