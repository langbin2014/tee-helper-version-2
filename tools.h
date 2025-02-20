#ifndef TOOLS_H
#define TOOLS_H

#include<QtDebug>
#include<QDataStream>
#include<QDateTime>
#include<cmath>
#include<QPoint>
#include<QJsonObject>
#include<QJsonDocument>
#include<QJsonArray>
struct Page {
    int q;
    int w;

    struct Channel{
        int q;
        int w;
    };

    enum Single_Data_Field{
        e1,e2

    };

    int get_q(){
        return q;
    }

    void set_q(int v)
    {
        q = v;
    }

    Channel channels[4];

    int get_channel_Data(int i, Single_Data_Field field)
    {
        auto cn = channels[i];
        if(field == e1)
            return  cn.q;
        return 0;
    }

    Channel set_channel_Data(int i,int v)
    {
        return channels[i];
    }

};

//struct T {
//    int q;
//    int w;

//    void update()
//    {
//        q_changed();
//        w_changed();
//    }

//    enum Bt{
//        save,read
//    };

//};



//struct T2{
//    int q;
//    int w;

//    void update()
//    {
//        q_changed();
//        w_changed();
//    }

//    enum Bt{
//        save,read,check
//    };
//};



//enum class BT{Save_Flash,Save_Ram,Read_Flash,Read_Ram};

//void button_pressed(std::string bt)
//{
//    if(bt == "固化")
//    {


//    }
//}


class Tools
{
public:
    Tools();
    //     inline  static QString  jsonObj2String(const QJsonObject &json)
    //    {
    //        return QString(QJsonDocument(json).toJson(QJsonDocument::Compact));
    //    }
    static   void jsonObj2String(const QJsonObject &json, QString &result)
    {
        result =  QString(QJsonDocument(json).toJson(QJsonDocument::Compact));
    }

    static  void jsonObj2String(const QJsonArray &json, QString &result)
    {
        result =  QString(QJsonDocument(json).toJson(QJsonDocument::Compact));
    }

    static    void str_2_json(const QString &str, QJsonObject &obj)
    {
        QJsonDocument jsonDocument = QJsonDocument::fromJson(str.toUtf8());
        if(jsonDocument.isNull())
        {
            qDebug() <<"解析失败:"+str;
            obj = QJsonObject();
            return ;
        }
        obj = jsonDocument.object();

    }


    static  void str_2_jsonarray(const QString &str, QJsonArray &arr)
    {
        QJsonDocument jsonDocument = QJsonDocument::fromJson(str.toUtf8());
        if(jsonDocument.isNull())
        {
            qDebug() <<"解析失败:"+str ;
            arr= QJsonArray();
            return ;
        }
        arr =   jsonDocument.array();
    }



    inline  static qint64 get_1970_ms()
    {
        return  QDateTime::currentDateTime().toMSecsSinceEpoch();
    }
    static QString get_pts_id(){
        return  QString::number(get_1970_ms());
    }
    static QString get_ini_file_name();
    static double get_angle(double x1,double y1,double z1,double angle_x,double angle_y,double angle_z,double x2,double y2,double z2);
    union intWithFloat
    {
        int m_i32;
        float m_f32;
    };
    static  inline  int swapInt32(int value)
    {
        return ((value & 0x000000FF) << 24) |
                ((value & 0x0000FF00) << 8) |
                ((value & 0x00FF0000) >> 8) |
                ((value & 0xFF000000) >> 24);
    }

    static  inline  unsigned int swap_u_Int32(unsigned int value)
    {
        return ((value & 0x000000FF) << 24) |
                ((value & 0x0000FF00) << 8) |
                ((value & 0x00FF0000) >> 8) |
                ((value & 0xFF000000) >> 24);
    }


    static  inline   float swapFloat32(float value)
    {
        static   union intWithFloat i;
        i.m_f32 = value;//因为输入的是float类型，用float接受不会改变数值
        //        float *pf = NULL;
        //        int *pi = NULL;
        //        pf = &i.m_f32;
        //        pi = &i.m_i32;
        i.m_i32 = swapInt32(i.m_i32);//用int型接受不会改变数值
        return i.m_f32;
    }


    static   int test()
    {
        float qq = 22;
        float   cc = swapFloat32(22);
        QByteArray data;
        QDataStream out(&data,QIODevice::WriteOnly);
        qDebug() << QByteArray((const char*)(&qq),4).toHex() << QByteArray((const char*)(&cc),4).toHex() << out.byteOrder() << is_little();
        return 0;
    }



    static bool save_txt(const uchar * buf,int len,const QString & file_name);
    static void open_txt_win32(const QString & file_name);

    enum class  Out_Order{
        Little,Big
    };
    union p{
        int a;
        char b;
    };

    static inline bool is_little(){
        p p1;
        p1.a = 1;
        return p1.a==p1.b;
    }

    static void create_range_0_9_data_to_str(char *str, int str_len)
    {
        str[str_len-1]='\0';
        int zore = '0';

        for(int i =0; i < str_len-1;i++)
        {
            str[i] = zore + rand()%10;
        }
    }


    static void make_path(const QString & path,int left);
    static QString to_cpp_file_path(const QString & path);
    static void save_list_to_xlsx(const QList<QString> list , const QString & path);
    static QString sec_to_min_sec(int sec);

    /////////////////////////////////////

    Q_INVOKABLE static void grap_screen(QImage & result);

    static void bmp_2_base64_string(const QImage & img,QString & result);
    static void base64_string_2_img(const QString & str,QImage & result);

    Q_INVOKABLE static QString get_mac_address();
    Q_INVOKABLE static QString getPicturesDir(const QString &node_dir);
    static  QString create_file_name(qint64 pts, const QString & dir_node, const QString &format);
    static QString get_today_path();
    static QString get_root_path();
    static bool save_file_to_dir_node(const QImage & pic,const QString & name, const  QStringList & dir_nodes);
    static void fixed(QByteArray  & need_fix, const QByteArray & dark,const QByteArray & light);
    static QString date_to_age_str(const QDateTime &date);
    static QString date_to_year(const QString &date);
    static QString date_to_month(const QString &date);
    static void test_dir();
private:
    const static QString _TAG_;
    const static bool _DEBUG_;
};

class SHA1Utils {
public:
    SHA1Utils(){}
    const   bool hexcase = false;
    const  QString b64pad = "=";
    const   int chrsz = 8;

    // 得到字符串SHA-1值的方法
    QString hex_sha1(QString s) {
        s = (s.isNull()) ? "" : s;
        return binb2hex(core_sha1(str2binb(s), s.length() * chrsz));
    }

    QString b64_hmac_sha1(QString key, QString data) {
        return binb2b64(core_hmac_sha1(key, data));
    }

    QString b64_sha1(QString s) {
        s = (s.isNull()) ? "" : s;
        return binb2b64(core_sha1(str2binb(s), s.length() * chrsz));
    }

    QString binb2b64(std::vector<int> binarray) {
        const char * tab = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz0123456789+/";
        QString str = "";
        binarray = strechbinarray(binarray, binarray.size() * 4);

        for (int i = 0; i < binarray.size() * 4; i += 3) {
            int triplet = (((binarray[i >> 2] >> 8 * (3 - i % 4)) & 0xff) << 16)
                    | (((binarray[i + 1 >> 2] >> 8 * (3 - (i + 1) % 4)) & 0xff) << 8)
                    | ((binarray[i + 2 >> 2] >> 8 * (3 - (i + 2) % 4)) & 0xff);

            for (int j = 0; j < 4; j++) {
                if (i * 8 + j * 6 > binarray.size() * 32) {
                    str += b64pad;
                } else {
                    str += tab[(triplet >> 6 * (3 - j)) & 0x3f];
                }
            }
        }

        return cleanb64str(str);
    }

    QString binb2hex(std::vector<int> binarray) {
        const char * hex_tab = hexcase ? "0123456789abcdef" : "0123456789abcdef";
        QString str = "";

        for (int i = 0; i < binarray.size() * 4; i++) {
            char a = (char) hex_tab[(binarray[i >> 2] >> ((3 - i % 4) * 8 + 4)) & 0xf];
            char b = (char) hex_tab[(binarray[i >> 2] >> ((3 - i % 4) * 8)) & 0xf];
            str += a;
            str += b;
        }

        return str;
    }

    QString binb2str(std::vector<int> bin) {
        QString str = "";
        int mask = (1 << chrsz) - 1;

        for (int i = 0; i < bin.size() * 32; i += chrsz) {
            str += (char) (((unsigned int)(bin[i >> 5]) >> (24 - i % 32)) & mask);
        }

        return str;
    }

    int bit_rol(int num, int cnt) {
        return (num << cnt) | ((unsigned int)(num) >> (32 - cnt));
    }

    QString cleanb64str(QString str) {
        str = (str.isNull()) ? "" : str;
        int len = str.length();

        if (len <= 1) {
            return str;
        }

        QChar a;
        char trailchar = str.at(len - 1).toLatin1();
        QString trailstr = "";

        for (int i = len - 1; i >= 0 && str.at(i) == trailchar; i--) {
            trailstr += str.at(i);
        }

        return str.mid(0, str.indexOf(trailstr));
    }

    std::vector<int>  complete216(std::vector<int> oldbin) {
        if (oldbin.size() >= 16) {
            return oldbin;
        }

        std::vector<int> newbin;// = new int[16 - oldbin.length];
        newbin.resize(16 - oldbin.size());

        for (int i = 0; i < newbin.size();  i++)
            newbin[i] = 0;

        return concat(oldbin, newbin);
    }

    std::vector<int>  concat(std::vector<int>  oldbin, std::vector<int>  newbin) {
        //        int[] retval = new int[oldbin.length + newbin.length];
        std::vector<int> retval;
        int s = oldbin.size() + newbin.size();
        retval.resize(s);

        for (int i = 0; i < s; i++) {
            if (i < oldbin.size()) {
                retval[i] = oldbin[i];
            } else {
                retval[i] = newbin[i - oldbin.size()];
            }
        }

        return retval;
    }

    std::vector<int>   core_hmac_sha1(QString key, QString data) {
        key = (key.isNull()) ? "" : key;
        data = (data.isNull()) ? "" : data;
        std::vector<int> bkey = complete216(str2binb(key));

        if (bkey.size() > 16) {
            bkey = core_sha1(bkey, key.length() * chrsz);
        }

        std::vector<int> ipad ;
        std::vector<int> opad ;
        ipad.resize(16);
        opad.resize(16);

        for (int i = 0; i < 16; ipad[i] = 0, opad[i] = 0, i++)
            ;

        for (int i = 0; i < 16; i++) {
            ipad[i] = bkey[i] ^ 0x36363636;
            opad[i] = bkey[i] ^ 0x5c5c5c5c;
        }

        std::vector<int> hash = core_sha1(concat(ipad, str2binb(data)), 512 + data.length() * chrsz);

        return core_sha1(concat(opad, hash), 512 + 160);
    }

    std::vector<int>core_sha1(std::vector<int> x, int len) {
        int size = (len >> 5);
        x = strechbinarray(x, size);
        x[len >> 5] |= 0x80 << (24 - len % 32);
        size = ((len + 64 >> 9) << 4) + 15;
        x = strechbinarray(x, size);
        x[((len + 64 >> 9) << 4) + 15] = len;

        std::vector<int> w;// = new int[80];
        w.resize(80);
        int a = 1732584193;
        int b = -271733879;
        int c = -1732584194;
        int d = 271733878;
        int e = -1009589776;

        for (int i = 0; i < x.size(); i += 16) {
            int olda = a;
            int oldb = b;
            int oldc = c;
            int oldd = d;
            int olde = e;

            for (int j = 0; j < 80; j++) {
                if (j < 16) {
                    w[j] = x[i + j];
                } else {
                    w[j] = rol(w[j - 3] ^ w[j - 8] ^ w[j - 14] ^ w[j - 16], 1);
                }

                int t = safe_add(safe_add(rol(a, 5), sha1_ft(j, b, c, d)), safe_add(safe_add(e, w[j]), sha1_kt(j)));

                e = d;
                d = c;
                c = rol(b, 30);
                b = a;
                a = t;
            }

            a = safe_add(a, olda);
            b = safe_add(b, oldb);
            c = safe_add(c, oldc);
            d = safe_add(d, oldd);
            e = safe_add(e, olde);
        }

        std::vector<int> retval ;//= new int[5];
        retval.resize(5);

        retval[0] = a;
        retval[1] = b;
        retval[2] = c;
        retval[3] = d;
        retval[4] = e;

        return retval;
    }

    void dotest() {
        QString key = "key";
        QString data = "data";
        qDebug() << ("hex_sha1(" + data + ")=" + hex_sha1(data));
        qDebug() <<("b64_sha1(" + data + ")=" + b64_sha1(data));
        qDebug() <<("str_sha1(" + data + ")=" + str_sha1(data));
        qDebug() <<("hex_hmac_sha1(" + key + "," + data + ")=" + hex_hmac_sha1(key, data));
        qDebug() <<("b64_hmac_sha1(" + key + "," + data + ")=" + b64_hmac_sha1(key, data));
        qDebug() <<("str_hmac_sha1(" + key + "," + data + ")=" + str_hmac_sha1(key, data));
    }

    QString hex_hmac_sha1(QString key, QString data) {
        return binb2hex(core_hmac_sha1(key, data));
    }

    int rol(int num, int cnt) {
        return (num << cnt) | ((unsigned int) (num) >> (32 - cnt));
    }

    int safe_add(int x, int y) {
        int lsw = (int) (x & 0xffff) + (int) (y & 0xffff);
        int msw = (x >> 16) + (y >> 16) + (lsw >> 16);

        return (msw << 16) | (lsw & 0xffff);
    }

    int sha1_ft(int t, int b, int c, int d) {
        if (t < 20)
            return (b & c) | ((~b) & d);

        if (t < 40)
            return b ^ c ^ d;

        if (t < 60)
            return (b & c) | (b & d) | (c & d);

        return b ^ c ^ d;
    }

    int sha1_kt(int t) {
        return (t < 20) ? 1518500249 : (t < 40) ? 1859775393 : (t < 60) ? -1894007588 : -899497514;
    }

    bool sha1_vm_test() {
        return hexcase ? hex_sha1("abc") == QString("a9993e364706816aba3e25717850c26c9cd0d89d") : hex_sha1("abc") == QString("a9993e364706816aba3e25717850c26c9cd0d89d");
    }

    QString str_hmac_sha1(QString key, QString data) {
        return binb2str(core_hmac_sha1(key, data));
    }

    QString str_sha1(QString s) {
        s = (s.isNull()) ? "" : s;

        return binb2str(core_sha1(str2binb(s), s.length() * chrsz));
    }

    std::vector<int> str2binb(QString str) {
        str = (str.isNull()) ? "" : str;

        std::vector<int> tmp ;//= new int[str.length() * chrsz];
        tmp.resize(str.length() * chrsz);
        int mask = (1 << chrsz) - 1;

        for (int i = 0; i < str.length() * chrsz; i += chrsz) {
            tmp[i >> 5] |= ((int) (str.at(i / chrsz)).toLatin1() & mask) << (24 - i % 32);
        }

        int len = 0;
        for (int i = 0; i < tmp.size() && tmp[i] != 0; i++, len++)
            ;

        std::vector<int> bin;// = new int[len];
        bin.resize(len);

        for (int i = 0; i < len; i++) {
            bin[i] = tmp[i];
        }

        return bin;
    }

    std::vector<int>  strechbinarray(std::vector<int> oldbin, int size) {
        int currlen = oldbin.size();

        if (currlen >= size + 1) {
            return oldbin;
        }

        std::vector<int> newbin;// = new int[size + 1];
        newbin.resize(size + 1);
        for (int i = 0; i < size; newbin[i] = 0, i++)
            ;

        for (int i = 0; i < currlen; i++) {
            newbin[i] = oldbin[i];
        }

        return newbin;
    }

    void main() {
        qDebug() << ("admin的SHA1的值为：" + hex_sha1("admin") + ",length=" + QString::number(hex_sha1("admin").length()));
        dotest();
        qDebug() <<  sha1_vm_test();
    }

};

#endif // TOOLS_H
