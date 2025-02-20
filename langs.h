
#ifndef LANGS_H
#define LANGS_H



#include<QList>
#include<map>
class Langs
{
    int index = 0;
public:
    Langs();
    void set_index(int index);
    void clear();
    QString my_tr(const QString & key);
    std::map <QString, std::map<QString,QString>> langs;
    std::map <QString, std::map<QString,QString>>::iterator it;
    int get_index(){
        return index;
    }
    const char * files[3] = {
        "0",
        ":/langs/english.txt",
        ":/langs/xibanya.txt"
    };
//    const char * files[3] = {
//        "0",
//        "english.txt",
//    };
};

#endif // LANGS_H
