#include "data_manager.h"
#include"../settings.h"
#include"c++/table/worker_table.h"
#include"patient_manager.h"
Data_Manager * Data_Manager::_instance = nullptr;
Data_Manager::Data_Manager( QObject *parent):QObject(parent),model(new Doctor_Model(this))
{
    QJsonArray workers = Worker_Table::get_instance()->query_all();
    model->reset(workers);
    //set_worker_model_json_array(workers);
    load_login_data();
    qDebug() << "----------" << workers;

}

void Data_Manager::load_login_data()
{
    //    auto setting = Settings::get_instance();
    //    if(setting->get_remember_password_bool())
    //    {
    //        set_account(setting->get_logined_worker_string());
    //        set_password(setting->get_worker_password_string());
    //    }

}

bool Data_Manager::login(const QString &account, const QString &password)
{
    if(account.isNull())
    {
        //  show_msg("请输入账号");
        return false;
    }

    if(password.isNull())
    {
        //   show_msg("请输入密码");
        return false;
    }

    auto w_table =  Worker_Table::get_instance();
    QString acc_key = w_table->get_field_name(Worker_Table::Fields::account_TEXT);
    QString pass_key = w_table->get_field_name(Worker_Table::Fields::password_TEXT);

    QJsonArray workers = model->get_all_info();
    for(int i =0; i < workers.size();i++)
    {
        QJsonObject work = workers.at(i).toObject();
        if(work[acc_key].toString() == account)
        {
            if(work[pass_key].toString() == password)
            {
                //  show_msg("登录成功");
                set_current_doctor_info(work); //设置成当前医生
                set_logined(true); //登录成功状态
                auto setting = Settings::get_instance();

                if(setting->get_remember_password_bool()) //记住密码账号设置,登录成功保存
                {
                    setting->set_logined_worker_string(account);
                    setting->set_worker_password_string(password);

                }
                PatientManager::get_instance()->set_current_doctor(account);
                PatientManager::get_instance()->load_patients(account);
                return true;
            }
        }
    }

    // show_msg("账号不存在，或者密码不对");
    return false;
}

QJsonObject Data_Manager::get_login_page_info()
{
    auto setting = Settings::get_instance();
    QJsonObject info;
    if(setting->get_remember_password_bool())
    {
        info["account"] = setting->get_logined_worker_string();
        info["password"] = setting->get_worker_password_string();
    }
    else
    {
        info["account"] = "";
        info["password"] = "";
    }
    return info;
}

Data_Manager *Data_Manager::get_instance()
{
    if(_instance == nullptr)
        _instance = new Data_Manager();
    return _instance;
}

QJsonObject Data_Manager::get_info(const QString &account)
{
    return model->get_info(account);
}

void Data_Manager::update_work(const QString &account, const QJsonObject &info)
{
    model->update(account,info);
    Worker_Table::get_instance()->update("account_TEXT='"+account+"'",info);
}

void Data_Manager::remove(const QString &account)
{
    model->remove(account);

    Worker_Table::get_instance()->remove("account_TEXT='"+account+"'");
}

bool Data_Manager::add(const QJsonObject &info)
{
    bool ok = model->append(info);
    if(ok)
        Worker_Table::get_instance()->add_worker(info);
    return ok;

}


