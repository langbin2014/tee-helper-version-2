#ifndef DEFINE_TOOL_H
#define DEFINE_TOOL_H
#include<cmath>

const static double __verly_small__ = 1e-5;
inline static bool __is_far__(double v1,double v2)
{
    return std::abs(v1-v2) > __verly_small__;
}

#define DEFINE_PRO(type,name,enum_name,toFunction,default_value) \
    private: \
    type m_##name; \
    bool m_##name##_inited = false; \
    Q_PROPERTY(type    name        READ get_##name  WRITE set_##name    NOTIFY name##Changed) \
    public: \
    type get_##name(){  \
    if(m_##name##_inited== false)    \
{   \
    QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat); \
    m_##name = settings.value(get_field_name(Fields:: enum_name ), default_value ). toFunction (); \
    m_##name##_inited = true; \
    return m_##name;    \
    }   \
    else  return m_##name; \
    } \
    void set_##name(const  type & value){\
    if(value != m_##name)   \
{   \
    m_##name = value;   \
    QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat); \
    auto f = Fields:: enum_name; \
    settings.setValue(get_field_name(f),value); \
    emit name##Changed();   \
    }   \
    }   \


#define DEFINE_PRO_NO_AUTO_GET_SET(type,name, default_value) \
    private: \
    type m_##name = default_value; \
    Q_PROPERTY(type    name       READ get_##name  WRITE set_##name    NOTIFY name##Changed); \
    public: \
    type get_##name(){  \
    return m_##name;    \
    }   \
    void set_##name(const  type & value){\
    if(value != m_##name)   \
{   \
    m_##name = value;   \
    emit name##Changed();   \
    }   \
    }   \


#define DEFINE_SIG(name) \
    void name##Changed(); \

#define INIT_PRO(toFunction,name,enum_name,defaule_value) \
    m_##name = settings.value(get_field_name(Fields::##enum_name),defaule_value).##toFunction(); \

#define DEFINE_PRO_1(type,name) \
    private: \
    Q_PROPERTY(type  name  READ get_##name  WRITE set_##name     NOTIFY name##Changed); \
    public: \
    type get_##name(){  \
    return name;    \
    }   \
    void set_##name(const  type & value){\
    if(value != name)   \
    {   \
    name = value;   \
    emit name##Changed();   \
    }   \
    }   \


#define DEFINE_PROS_3(type,arr_name) \
    private: \
    Q_PROPERTY(type    arr_name##_0      READ get_##arr_name##_0   WRITE set_##arr_name##_0     NOTIFY arr_name##_0##Changed); \
    Q_PROPERTY(type    arr_name##_1      READ get_##arr_name##_1   WRITE set_##arr_name##_1     NOTIFY arr_name##_1##Changed); \
    Q_PROPERTY(type    arr_name##_2      READ get_##arr_name##_2   WRITE set_##arr_name##_2     NOTIFY arr_name##_2##Changed); \
    public: \
    type get_##arr_name##_0(){  \
    return arr_name[0];    \
    }   \
    type get_##arr_name##_1(){  \
    return arr_name[1];    \
    }   \
    type get_##arr_name##_2(){  \
    return arr_name[2];    \
    }   \
    void set_##arr_name##_0(const  type & value){\
    if(value != arr_name[0])   \
    {   \
    arr_name[0] = value;   \
    emit arr_name##_0##Changed();   \
    }   \
    }   \
    void set_##arr_name##_1(const  type & value){\
    if(value != arr_name[1])   \
    {   \
    arr_name[1] = value;   \
    emit arr_name##_1##Changed();   \
    }   \
    }   \
    void set_##arr_name##_2(const  type & value){\
    if(value != arr_name[2])   \
    {   \
    arr_name[2] = value;   \
    emit arr_name##_2##Changed();   \
    }   \
    }   \

#define DEFINE_SIGS_3(arr_name) \
    void arr_name##_0##Changed(); \
    void arr_name##_1##Changed(); \
    void arr_name##_2##Changed(); \

#define EMIT_SIG_1(name) \
    emit name##Changed(); \

#define EMIT_SIGS_3(arr_name) \
    emit arr_name##_0##Changed(); \
    emit arr_name##_1##Changed(); \
    emit arr_name##_2##Changed(); \



#define DEFINE_PROS_4(type,arr_name) \
    private: \
    Q_PROPERTY(type    arr_name##_0      READ get_##arr_name##_0   WRITE set_##arr_name##_0     NOTIFY arr_name##_0##Changed); \
    Q_PROPERTY(type    arr_name##_1      READ get_##arr_name##_1   WRITE set_##arr_name##_1     NOTIFY arr_name##_1##Changed); \
    Q_PROPERTY(type    arr_name##_2      READ get_##arr_name##_2   WRITE set_##arr_name##_2     NOTIFY arr_name##_2##Changed); \
    Q_PROPERTY(type    arr_name##_3      READ get_##arr_name##_3   WRITE set_##arr_name##_3     NOTIFY arr_name##_3##Changed); \
    public: \
    type get_##arr_name##_0(){  \
    return arr_name[0];    \
    }   \
    type get_##arr_name##_1(){  \
    return arr_name[1];    \
    }   \
    type get_##arr_name##_2(){  \
    return arr_name[2];    \
    }   \
    type get_##arr_name##_3(){  \
    return arr_name[3];    \
    }   \
    void set_##arr_name##_0(const  type & value){\
    if(value != arr_name[0])   \
    {   \
    arr_name[0] = value;   \
    emit arr_name##_0##Changed();   \
    }   \
    }   \
    void set_##arr_name##_1(const  type & value){\
    if(value != arr_name[1])   \
    {   \
    arr_name[1] = value;   \
    emit arr_name##_1##Changed();   \
    }   \
    }   \
    void set_##arr_name##_2(const  type & value){\
    if(value != arr_name[2])   \
    {   \
    arr_name[2] = value;   \
    emit arr_name##_2##Changed();   \
    }   \
    }   \
    void set_##arr_name##_3(const  type & value){\
    if(value != arr_name[3])   \
    {   \
    arr_name[3] = value;   \
    emit arr_name##_3##Changed();   \
    }   \
    }   \


#define DEFINE_SIGS_4(arr_name) \
    void arr_name##_0##Changed(); \
    void arr_name##_1##Changed(); \
    void arr_name##_2##Changed(); \
    void arr_name##_3##Changed(); \



#define EMIT_SIGS_4(arr_name) \
    emit arr_name##_0##Changed(); \
    emit arr_name##_1##Changed(); \
    emit arr_name##_2##Changed(); \
    emit arr_name##_3##Changed(); \






#define DEFINE_PROS_3_for_2_level_array(type,arr_name) \
    private: \
    Q_PROPERTY(type    arr_name##_0      READ get_##arr_name##_0   WRITE set_##arr_name##_0     NOTIFY arr_name##_0##Changed); \
    Q_PROPERTY(type    arr_name##_1      READ get_##arr_name##_1   WRITE set_##arr_name##_1     NOTIFY arr_name##_1##Changed); \
    Q_PROPERTY(type    arr_name##_2      READ get_##arr_name##_2   WRITE set_##arr_name##_2     NOTIFY arr_name##_2##Changed); \
    public: \
    type get_##arr_name##_0(){  \
    int i = get_channel_index(); \
    return arr_name[i][0];    \
    }   \
    type get_##arr_name##_1(){  \
    int i = get_channel_index(); \
    return arr_name[i][1];    \
    }   \
    type get_##arr_name##_2(){  \
        int i = get_channel_index(); \
    return arr_name[i][2];    \
    }   \
    void set_##arr_name##_0(const  type & value){\
        int i = get_channel_index(); \
    if(value != arr_name[i][0])   \
    {   \
    arr_name[i][0] = value;   \
    emit arr_name##_0##Changed();   \
    }   \
    }   \
    void set_##arr_name##_1(const  type & value){\
    int i = get_channel_index(); \
    if(value != arr_name[i][1])   \
    {   \
    arr_name[i][1] = value;   \
    emit arr_name##_1##Changed();   \
    }   \
    }   \
    void set_##arr_name##_2(const  type & value){\
    int i = get_channel_index(); \
    if(value != arr_name[i][2])   \
    {   \
    arr_name[i][2] = value;   \
    emit arr_name##_2##Changed();   \
    }   \
    }   \


#define DEFINE_PRO_1_for_1_level_array(type,arr_name) \
    Q_PROPERTY(type    arr_name      READ get_##arr_name  WRITE set_##arr_name    NOTIFY arr_name##Changed); \
    public: \
    type get_##arr_name(){  \
    int i = get_channel_index(); \
    return arr_name[i];    \
    }   \
    void set_##arr_name(const  type & value){\
        int i = get_channel_index(); \
    if(value != arr_name[i])   \
    {   \
    arr_name[i] = value;   \
    emit arr_name##Changed();   \
    }   \
    }   \































#define DEFINE_PROS_3_DOUBLE(type,arr_name) \
    private: \
    Q_PROPERTY(type    arr_name##_0      READ get_##arr_name##_0   WRITE set_##arr_name##_0     NOTIFY arr_name##_0##Changed); \
    Q_PROPERTY(type    arr_name##_1      READ get_##arr_name##_1   WRITE set_##arr_name##_1     NOTIFY arr_name##_1##Changed); \
    Q_PROPERTY(type    arr_name##_2      READ get_##arr_name##_2   WRITE set_##arr_name##_2     NOTIFY arr_name##_2##Changed); \
    public: \
    type get_##arr_name##_0(){  \
    return arr_name[0];    \
    }   \
    type get_##arr_name##_1(){  \
    return arr_name[1];    \
    }   \
    type get_##arr_name##_2(){  \
    return arr_name[2];    \
    }   \
    void set_##arr_name##_0(const  type & value){\
    if(__is_far__(value , arr_name[0]))   \
    {   \
    arr_name[0] = value;   \
    emit arr_name##_0##Changed();   \
    }   \
    }   \
    void set_##arr_name##_1(const  type & value){\
    if(__is_far__(value , arr_name[1]))   \
    {   \
    arr_name[1] = value;   \
    emit arr_name##_1##Changed();   \
    }   \
    }   \
    void set_##arr_name##_2(const  type & value){\
    if(__is_far__(value , arr_name[2]))   \
    {   \
    arr_name[2] = value;   \
    emit arr_name##_2##Changed();   \
    }   \
    }   \


#define DEFINE_PROS_4_DOUBLE(type,arr_name) \
    private: \
    Q_PROPERTY(type    arr_name##_0      READ get_##arr_name##_0   WRITE set_##arr_name##_0     NOTIFY arr_name##_0##Changed); \
    Q_PROPERTY(type    arr_name##_1      READ get_##arr_name##_1   WRITE set_##arr_name##_1     NOTIFY arr_name##_1##Changed); \
    Q_PROPERTY(type    arr_name##_2      READ get_##arr_name##_2   WRITE set_##arr_name##_2     NOTIFY arr_name##_2##Changed); \
    Q_PROPERTY(type    arr_name##_3      READ get_##arr_name##_3   WRITE set_##arr_name##_3     NOTIFY arr_name##_3##Changed); \
    public: \
    type get_##arr_name##_0(){  \
    return arr_name[0];    \
    }   \
    type get_##arr_name##_1(){  \
    return arr_name[1];    \
    }   \
    type get_##arr_name##_2(){  \
    return arr_name[2];    \
    }   \
    type get_##arr_name##_3(){  \
    return arr_name[3];    \
    }   \
    void set_##arr_name##_0(const  type & value){\
    if(__is_far__(value ,arr_name[0]))   \
    {   \
    arr_name[0] = value;   \
    emit arr_name##_0##Changed();   \
    }   \
    }   \
    void set_##arr_name##_1(const  type & value){\
    if(__is_far__(value , arr_name[1]))   \
    {   \
    arr_name[1] = value;   \
    emit arr_name##_1##Changed();   \
    }   \
    }   \
    void set_##arr_name##_2(const  type & value){\
    if(__is_far__(value , arr_name[2]))   \
    {   \
    arr_name[2] = value;   \
    emit arr_name##_2##Changed();   \
    }   \
    }   \
    void set_##arr_name##_3(const  type & value){\
    if(__is_far__(value , arr_name[3]))   \
    {   \
    arr_name[3] = value;   \
    emit arr_name##_3##Changed();   \
    }   \
    }   \


#define DEFINE_PRO_1_DOUBLE(type,name) \
    private: \
    Q_PROPERTY(type  name  READ get_##name  WRITE set_##name     NOTIFY name##Changed); \
    public: \
    type get_##name(){  \
    return name;    \
    }   \
    void set_##name(const  type & value){\
    if(__is_far__(value , name))   \
    {   \
    name = value;   \
    emit name##Changed();   \
    }   \
    }   \


#define DEFINE_PROS_3_for_2_level_array_DOUBLE(type,arr_name) \
    private: \
    Q_PROPERTY(type    arr_name##_0      READ get_##arr_name##_0   WRITE set_##arr_name##_0     NOTIFY arr_name##_0##Changed); \
    Q_PROPERTY(type    arr_name##_1      READ get_##arr_name##_1   WRITE set_##arr_name##_1     NOTIFY arr_name##_1##Changed); \
    Q_PROPERTY(type    arr_name##_2      READ get_##arr_name##_2   WRITE set_##arr_name##_2     NOTIFY arr_name##_2##Changed); \
    public: \
    type get_##arr_name##_0(){  \
    int i = get_channel_index(); \
    return arr_name[i][0];    \
    }   \
    type get_##arr_name##_1(){  \
    int i = get_channel_index(); \
    return arr_name[i][1];    \
    }   \
    type get_##arr_name##_2(){  \
        int i = get_channel_index(); \
    return arr_name[i][2];    \
    }   \
    void set_##arr_name##_0(const  type & value){\
        int i = get_channel_index(); \
    if(__is_far__(value , arr_name[i][0]))   \
    {   \
    arr_name[i][0] = value;   \
    emit arr_name##_0##Changed();   \
    }   \
    }   \
    void set_##arr_name##_1(const  type & value){\
    int i = get_channel_index(); \
    if(__is_far__(value , arr_name[i][1]))   \
    {   \
    arr_name[i][1] = value;   \
    emit arr_name##_1##Changed();   \
    }   \
    }   \
    void set_##arr_name##_2(const  type & value){\
    int i = get_channel_index(); \
    if(__is_far__(value , arr_name[i][2]))   \
    {   \
    arr_name[i][2] = value;   \
    emit arr_name##_2##Changed();   \
    }   \
    }   \


#define DEFINE_PRO_1_for_1_level_array_DOUBLE(type,arr_name) \
    Q_PROPERTY(type    arr_name      READ get_##arr_name  WRITE set_##arr_name    NOTIFY arr_name##Changed); \
    public: \
    type get_##arr_name(){  \
    int i = get_channel_index(); \
    return arr_name[i];    \
    }   \
    void set_##arr_name(const  type & value){\
        int i = get_channel_index(); \
    if(__is_far__(value , arr_name[i]))   \
    {   \
    arr_name[i] = value;   \
    emit arr_name##Changed();   \
    }   \
    }   \

#endif // DEFINE_TOOL_H
