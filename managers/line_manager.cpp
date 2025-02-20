#include "line_manager.h"
#include<core/image_core.h>
#include"c++/tools.h"
#include"c++/configure.h"
#include"c++/log.h"
LineManager * LineManager::_single_instace = nullptr;

LineManager::LineManager(QObject *parent):QObject(parent)
{
    QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat);
    _current_line_color =  settings.value("_current_line_color","#9DFF03").toString();
    _current_angle_color = settings.value("_current_angle_color","#ff55ff").toString();
    _current_pen_color = settings.value("_current_pen_color","#ff0000").toString();
    connect(this,&LineManager::line_unitChanged,this,&LineManager::update_all_line_display_text);
    connect(this,&LineManager::hor_per_pixel_mmChanged,this,&LineManager::update_all_line_display_text);
    connect(this,&LineManager::ver_per_pixel_mmChanged,this,&LineManager::update_all_line_display_text);
}

bool LineManager::mouse_event(const QString &picture_id, qint16 x, qint16 y, int event)
{



    if(picture_id != _current_picture_id)
    {
        Log::i(TAG,"线管理器的图片还没有切换,无视鼠标事件");
        return false;
    }
     if(picture_id ==  "-1")
         return false;
    fix(x,y);
    if(event == MouseEvent::MoveOnPressed) //MouseTask::NewLine,MouseTask::ChangePoint,MouseTask::MoveLine
        return   mouse_move_on_pressed( x, y);
    else if(event == MouseEvent::MoveNoPressed)
        return   mouse_move_no_pressed( x, y);
    else if(event == MouseEvent::Clicked) //MouseTask::NewLine,MouseTask::NoMouseTask
        return   mouse_clicked(x, y);
    else if(event == MouseEvent::Pressed) //MouseTask::ChangePoint,MouseTask::MoveLine
        return  mouse_pressed(x, y);
    else if(event == MouseEvent::Release) //MouseTask::ChangePoint,MouseTask::MoveLine
        return   mouse_release(x, y);
    else
        return false;
}

//void LineManager::reset_field_for_one_current_picture()
//{
//    _current_task_line = nullptr; //当前操作的线
//    _current_clicked_index_pos = -1;//当点击到某条的点时点的索引，画线，和修改点时会频繁使用此参数
//    setMouse_task(int(MouseTask::NoMouseTask));
//    setLine_task(int(LineTask::NoLineTask));
//    _old_line_moving_pos={0,0};
//}


bool LineManager::mouse_move_on_pressed( qint16 x, qint16 y)
{
    if (_line_task == LineTask::Pen) {//画笔功能
        _current_task_pen->line_data.push_back({x,y});
        _cur_polygon.append(QPoint(x, y));
        _cur_painter_path = QPainterPath();
        _cur_painter_path.addPolygon(_cur_polygon);
        return true;
    }
    if(_line_task == LineTask::NoLineTask && _mouse_task == MouseTask::NoMouseTask)
    {
        if(DEBUG)
        {
            QString info;
            if(_line_task == LineTask::NoLineTask)
                info = "无线任务";
            if(_mouse_task == MouseTask::NoMouseTask )
            {
                if(info.isNull())
                    info = "无鼠标任务";
                else
                    info = info + ",且无鼠标任务";
            }
            Log::d(TAG,info+",无视鼠标按下且移动事件");
        }
        return false;
    }
    else if(_mouse_task == MouseTask::MoveLine)
    {
        qint16 x_offset = _old_line_moving_pos.x - x;
        qint16 y_offset = _old_line_moving_pos.y - y;
        QMutexLocker l(&_current_task_line->line_data_read_write_lock);
        for(auto it =_current_task_line->line_data.begin(); it != _current_task_line->line_data.end();it++)
        {
            it->x -= x_offset;
            it->y -= y_offset;
        }
        _old_line_moving_pos = {x,y};
        if(DEBUG)
            Log::d(TAG,"鼠标移动，正在移动线任务");
        setShow_cross_mouse(true);
        emit line_display_text_changed(_current_task_line->line_id,get_line_display_text(*_current_task_line));
        return true;
    }
    else if(_mouse_task == MouseTask::ChangePoint)
    {
        if(_current_clicked_index_pos < 0)
        {
            Log::e(TAG,"代码逻辑异常,移动点任务时，移动的点索引小于0:"+QString::number(_current_clicked_index_pos));
            return false;
        }
        if(_current_clicked_index_pos >= _current_task_line->line_data.size())
        {
            Log::e(TAG,"代码逻辑异常,移动点任务时，移动的点索引大于等于线数组:"+QString::number(_current_clicked_index_pos));
            return false;
        }
        QMutexLocker l(&_current_task_line->line_data_read_write_lock);
        _current_task_line->line_data[_current_clicked_index_pos] = {x,y};
        setShow_cross_mouse(true);
        emit line_display_text_changed(_current_task_line->line_id,get_line_display_text(*_current_task_line));
        return true;
    }
    else
    {//else if(_mouse_task == MouseTask::NewLine) //无视此信号
        setShow_cross_mouse(true);
        return false;
    }
}

bool LineManager::mouse_move_no_pressed( qint16 x, qint16 y)
{

    if(_line_task == LineTask::NoLineTask || _line_task == LineTask::Pen)
    {
        if(DEBUG)
            Log::d(TAG,"无线任务,无视鼠标移动事件");
        check_mouse_is_near_line(x,y);
        return false;
    }

    if(_mouse_task == MouseTask::NoMouseTask )
    {
        if(DEBUG)
            Log::d(TAG,"无鼠标任务,无视鼠标移动事件");
        check_mouse_is_near_line(x,y);
        return false;
    }
    else if(_mouse_task == MouseTask::NewLine)
    {
        if(_current_clicked_index_pos < 0)
        {
            Log::e(TAG,"代码逻辑异常,新线鼠标移动时，移动的点索引小于0:"+QString::number(_current_clicked_index_pos));
            return false;
        }
        if(_current_task_line == nullptr)
        {
            Log::e(TAG,"代码逻辑异常,新线鼠标移动时，没有创建顶点数组");
            return false;
        }
        if(_current_clicked_index_pos >= _current_task_line->line_data.size())
        {
            Log::e(TAG,"代码逻辑异常,新线鼠标移动时，移动的点索引大于等于线数组:"+QString::number(_current_clicked_index_pos));
            return false;
        }
        QMutexLocker l(&_current_task_line->line_data_read_write_lock);
        _current_task_line->line_data[_current_clicked_index_pos] = {x,y};

        if(DEBUG)
            Log::d(TAG,"新线任务,鼠标移动事件顶点已更新");
        setShow_cross_mouse(true);
        return true;
    }
    else
    {
        //ChangePoint,无视
        //MoveLine,无视
        return false;
    }
}



bool LineManager::mouse_clicked( qint16 x, qint16 y)
{
    if(DEBUG)
        qDebug() << "点击1111";
    if (_line_task == LineTask::Pen) {
        //如果是画笔状态，则无视点击事件
        return true;
    }
    if(_mouse_task == MouseTask::NewLine)
    { //追加一个点到新线
        if(_current_task_line == nullptr)
        {
            Log::e(TAG,"代码异常,新线任务时，当前线为 nullptr");
            return false;
        }
        QMutexLocker l(&_current_task_line->line_data_read_write_lock);

        _current_task_line->line_data.push_back({x,y});
        _current_clicked_index_pos = _current_task_line->line_data.size()-1;
        if(DEBUG)
            Log::d(TAG,"处于NewLine鼠标点击事件,且有线任务，追加一个坐标");
        //        for(auto it = _current_task_line->line_data.begin(); it != _current_task_line->line_data.end();it++)
        //            qDebug() << "("<<it->x << "," << it->y <<")";
        return true;
    }
    else if(_mouse_task == MouseTask::NoMouseTask)
    {
        if(DEBUG)
            qDebug() << "点击2222";
        if(_line_task == LineTask::Line || _line_task == LineTask::Angle)
        {//如果左边的线或者角度按钮开启了，则生成一条新的线
            if(DEBUG)
                qDebug() << "点击33333";
            std::shared_ptr<LineTypesNS::Line> new_line(new LineTypesNS::Line);
            new_line->line_data.push_back({x,y});
            qint16 x2 = x+1;
            qint16 y2 = y+1;
            new_line->line_data.push_back({x2,y2});
            new_line->patient_id = _current_patient_id;
            new_line->image_id = _current_picture_id;
            new_line->line_id = Tools::get_1970_ms();
            if(_line_task == LineTask::Angle)
                new_line->line_color = _current_angle_color; // Configure::Line_Color ;
            else
                new_line->line_color = _current_line_color;
            new_line->line_type = _line_task == LineTask::Line ? LineTypesNS::Type::Normal_Line : LineTypesNS::Type::Angle_Line;
            //移动的时候回更新这个点 //点击这固定这个点，且追加多一个相同的点

            _current_picture_lines->push_back(new_line);
            //撤销功能 有新增,去掉可以反撤销部分
            while (_undo_index < _undo_list.count() - 1) {
                if (_undo_list.last() == Operate::Delete) {
                    _current_undo_lines->erase(--_current_undo_lines->end());
                }
                _undo_list.removeLast();
            }
            _undo_list.append(Operate::Insert);
            ++_undo_index;
            setLine_redo(false);
            setLine_undo(true);

            _current_task_line = new_line;
            emit setChecked_line_id(-1);
            _current_clicked_index_pos = _current_task_line->line_data.size()-1;
            setMouse_task(int(MouseTask::NewLine));
            if(DEBUG)
                Log::d(TAG,"无鼠标事件,且有线任务，开始一条新线");
            return true;
        }else if(_line_task == LineTask::NoLineTask)
        { //好像是属于pressed事件
            if(!_releasing_for_end_move_line_or_change_point)
                empty_checked_line();
            return false; //
        }
        else //不可能到达
            return false;
    }
    else
    {
        //MouseTask::ChangePoint,
        //MouseTask::MoveLine, 不需要处理
        if(DEBUG)
            Log::d(TAG,"MouseTask::ChangePoint,MouseTask::MoveLine点击事件不需要处理");
        return false;
    }
}

bool LineManager::mouse_pressed( qint16 x, qint16 y)
{
    //画笔功能
    if (_line_task == LineTask::Pen ) {
        std::shared_ptr<LineTypesNS::Line> new_line(new LineTypesNS::Line);
        new_line->line_data.push_back({x,y});
        new_line->patient_id = _current_patient_id;
        new_line->image_id = _current_picture_id;
        new_line->line_id = Tools::get_1970_ms();
        new_line->line_color = _current_pen_color;
        new_line->line_type = LineTypesNS::Type::Pen;
        _current_task_pen = new_line;
//        _painter_path_color.append(_current_pen_color);
        //方便line_view,快速绘制
        _cur_polygon.append(QPoint(x, y));
        _cur_painter_path.addPolygon(_cur_polygon);
        return  true;
    }
    if(_line_task == LineTask::Line || _line_task == LineTask::Angle)
    {
        if(DEBUG)
            Log::d(TAG,"处于线和角度任务，取消移动线和移动点任务的pressed事件");
        return false;
    }

    if(_mouse_task == MouseTask::NewLine)
    {
        if(DEBUG)
            Log::d(TAG,"正在新线任务,取消触发MouseTask::ChangePoint,MouseTask::MoveLine事件");
        return false;
    }

    //    auto this_pictrue_all_line_it = _all_lines.find(picture_id);
    //    if(this_pictrue_all_line_it == _all_lines.end())
    //    {
    //        Log::i(TAG,"图片id找不到线集合,创建一个");
    //        auto temp   = std::shared_ptr<std::list<std::shared_ptr<LineTable::Line>>>(new std::list<std::shared_ptr<LineTable::Line>>());
    //        _all_lines[picture_id] = temp;
    //        _current_picture_lines = temp;
    //    }
    //    else
    //        _current_picture_lines = this_pictrue_all_line_it->second;


    //    int ro = _rotate_factor < 0 ?  -_rotate_factor : _rotate_factor;
    //    bool need_fix = ro  % 180 != 0; //需要修复
    for(auto it = _current_picture_lines->begin(); it != _current_picture_lines->end();it++)
    {
        QMutexLocker l(&((*it)->line_data_read_write_lock));
        std::vector<LineTypesNS::Point> &line = (*it)->line_data;
        for(int i =0 ;i < line.size() ;i++)
        {
            qint16 x1 = line[i].x;
            qint16 y1 = line[i].y;
            //∣AB∣=√[(x1－x2)²+(y1－y2)²]。
            double two_point_dis =  std::sqrt(std::pow(x1 -x,2) + std::pow(y1-y,2));
            if(two_point_dis < LineTypesNS::MAX_DIS/_scale_factor) //暂时不用存到数据库
            {
                _current_task_line = *it;
                emit setChecked_line_id(_current_task_line->line_id);
                _current_clicked_index_pos = i;
                setMouse_task(int(MouseTask::ChangePoint));
                if(DEBUG)
                    Log::d(TAG,"无鼠标任务，无线任务，但点到点,开始 MouseTask::ChangePoint任务");
                return false; //好像现在还不需要刷新
            }
        }
    }

    for(auto it = _current_picture_lines->begin(); it != _current_picture_lines->end();it++)
    {
        QMutexLocker l(&((*it)->line_data_read_write_lock));
        std::vector<LineTypesNS::Point> &line = (*it)->line_data;

        for(int i =0 ;i + 1 < line.size() ;i++)
        {
            qint16 x1 = line[i].x;
            qint16 y1 = line[i].y;
            qint16 x2 = line[i+1].x;
            qint16 y2 = line[i+1].y;
            float point_to_line_dis = Core::ImageTool::get_point_to_line_dis(x,y,x1,y1,x2,y2);
            if(DEBUG)
                qDebug() << "(" << x1<<","<<y1<<")," <<"(" << x2<<","<<y2<<")" <<",(" << x<<","<<y<<")" << " dis = " << int(point_to_line_dis);
            if(point_to_line_dis < LineTypesNS::MAX_DIS /_scale_factor) //暂时不用存到数据库
            {
                qint16 max_x = std::max(x1,x2);
                qint16 min_x = std::min(x1,x2);
                qint16 max_y = std::max(y1,y2);
                qint16 min_y = std::min(y1,y2);
                if(x < max_x && x > min_x && y < max_y && y > min_y)
                {
                    _current_task_line = *it;
                    emit setChecked_line_id(_current_task_line->line_id);
                    _current_clicked_index_pos = -1;
                    setMouse_task(int(MouseTask::MoveLine));
                    _old_line_moving_pos={x,y};
                    if(DEBUG)
                        Log::d(TAG,"无鼠标任务，无线任务，但点到线,开始 MouseTask::ChangePoint任务");
                    return false;
                }
            }
        }
    }
    if(DEBUG)
        Log::d(TAG,"无鼠标任务，无线任务，且点到空白,不做任何操作");
    return false;
}

bool LineManager::mouse_release( qint16 x, qint16 y)
{
    _releasing_for_end_move_line_or_change_point = false;
    if (_line_task == LineTask::Pen) {//画笔功能
        //QPainterPath path;
        // 将当前path数据添加到path中
        //path.addPolygon(_cur_polygon);
        // 将创建好的对象追加到存储容器中
        //_painter_path_list.append(path);
        // 当前对象赋值为空path
        _cur_painter_path.clear();
        _cur_painter_path = QPainterPath();
        // 清空path数据容器
        _cur_polygon.clear();
        if(saveNewPenData()) {
            _current_picture_lines->push_back(_current_task_pen);
            //撤销功能 有新增,去掉可以反撤销部分
            while (_undo_index < _undo_list.count() - 1) {
                if (_undo_list.last() == Operate::Delete) {
                    _current_undo_lines->erase(--_current_undo_lines->end());
                }
                _undo_list.removeLast();
            }
            _undo_list.append(Operate::Insert);
            ++_undo_index;
            setLine_redo(false);
            setLine_undo(true);

            QJsonObject info = line_to_json(*_current_task_pen);
            emit new_line(info);
            _current_task_pen = nullptr;
        }
    }
    if(_mouse_task == MouseTask::MoveLine)
    {
        qint16 x_offset = _old_line_moving_pos.x - x;
        qint16 y_offset = _old_line_moving_pos.y - y;
        QMutexLocker l(&_current_task_line->line_data_read_write_lock);
        for(auto it =_current_task_line->line_data.begin(); it != _current_task_line->line_data.end();it++)
        {
            it->x += x_offset;
            it->y += y_offset;
        }
        if(DEBUG)
            Log::d(TAG,"移动线任务释放，即将把数据存到数据库");
        //保存到数据库,待实现
        end_move_line();
        _releasing_for_end_move_line_or_change_point = true;
        return true;
    }
    else if(_mouse_task == MouseTask::ChangePoint)
    {
        if(_current_clicked_index_pos < 0)
        {
            Log::e(TAG,"代码逻辑异常,释放点任务时，移动的点索引小于0:"+QString::number(_current_clicked_index_pos));
            return false;
        }
        if(_current_clicked_index_pos >= _current_task_line->line_data.size())
        {
            Log::e(TAG,"代码逻辑异常,释放点任务时，移动的点索引大于等于线数组:"+QString::number(_current_clicked_index_pos));
            return false;
        }
        QMutexLocker l(&_current_task_line->line_data_read_write_lock);
        _current_task_line->line_data[_current_clicked_index_pos] = {x,y};
        end_change_point();
        _releasing_for_end_move_line_or_change_point = true;
        if(DEBUG)
            Log::d(TAG,"移动点任务释放，即将把点数据存到数据库");
        //保存到数据库,待实现
        return true;
    }
    //新线时不需要做任何操作
    return true;
}

void LineManager::check_mouse_is_near_line(qint16 x, qint16 y)
{
    if(_current_picture_lines == nullptr)
    {
        setShow_cross_mouse(false);
        return;
    }
    //    int ro = _rotate_factor < 0 ?  -_rotate_factor : _rotate_factor;
    //    bool need_fix = ro  % 180 != 0; //需要修复
    for(auto it = _current_picture_lines->begin(); it != _current_picture_lines->end();it++)
    {
        QMutexLocker l(&((*it)->line_data_read_write_lock));
        std::vector<LineTypesNS::Point> &line = (*it)->line_data;
        for(int i =0 ;i < line.size() ;i++)
        {
            qint16 x1 = line[i].x;
            qint16 y1 = line[i].y;
            //∣AB∣=√[(x1－x2)²+(y1－y2)²]。

            double two_point_dis =  std::sqrt(std::pow(x1 -x,2) + std::pow(y1-y,2));
            if(two_point_dis < LineTypesNS::MAX_DIS /_scale_factor) //暂时不用存到数据库
            {
                setShow_cross_mouse(true);
                return;
            }
        }
    }

    for(auto it = _current_picture_lines->begin(); it != _current_picture_lines->end();it++)
    {
        QMutexLocker l(&((*it)->line_data_read_write_lock));
        std::vector<LineTypesNS::Point> &line = (*it)->line_data;

        for(int i =0 ;i + 1 < line.size() ;i++)
        {
            qint16 x1 = line[i].x;
            qint16 y1 = line[i].y;
            qint16 x2 = line[i+1].x;
            qint16 y2 = line[i+1].y;

            float point_to_line_dis = Core::ImageTool::get_point_to_line_dis(x,y,x1,y1,x2,y2);
            if(point_to_line_dis < LineTypesNS::MAX_DIS / _scale_factor) //暂时不用存到数据库
            {
                qint16 max_x = std::max(x1,x2);
                qint16 min_x = std::min(x1,x2);
                qint16 max_y = std::max(y1,y2);
                qint16 min_y = std::min(y1,y2);
                if(x < max_x && x > min_x && y < max_y && y > min_y)
                {
                    setShow_cross_mouse(true);
                    return;
                }
            }
        }
    }

    setShow_cross_mouse(false);

}

QJsonObject LineManager::line_to_json(const LineTypesNS::Line &line)
{
    QJsonObject ele;
    ele["ID_LongLong"] = line.line_id;
    ele["Type_integer"] = int(line.line_type);
    ele["Color_TEXT"] = line.line_color;
    // ele["Checked_Bool"] = true;
    if (line.line_type == LineTypesNS::Type::Pen) {
        ele["Display_TEXT"] = tr("Brush line");
    } else {
        ele["Display_TEXT"] = get_line_display_text(line);
    }
    return ele;
}

QString LineManager::get_line_display_text(const LineTypesNS::Line &line)
{
    auto & points = line.line_data;
    if(line.line_type == LineTypesNS::Type::Normal_Line)
    {
        float len =0;
        for(auto it = points.begin();it != points.end();it++)
        {
            auto next = it+1;
            if(next == points.end())
                break;

            qint16 pixel_x1 = it->x;
            qint16 pixel_y1 = it->y;
            qint16 pixel_x2 = next->x;
            qint16 pixel_y2 = next->y;

            if(_line_unit == LineTypesNS::Unit::Px)
            {
                float two_point_dis_px =  std::sqrt(std::pow(pixel_x1 -pixel_x2,2) + std::pow(pixel_y1-pixel_y2,2));
                len += int(two_point_dis_px);
            }
            else {
                float x1 = pixel_x1 * _hor_per_pixel_mm;
                float x2 = pixel_x2 * _hor_per_pixel_mm;
                float y1 = pixel_y1 * _ver_per_pixel_mm;
                float y2 = pixel_y2 * _ver_per_pixel_mm;
                float two_point_dis_mm =  std::sqrt(std::pow(x1 -x2,2) + std::pow(y1-y2,2));
                float result = two_point_dis_mm;
                if(_line_unit == LineTypesNS::Unit::Cm)
                    result = result / 10.0f;
                else if(_line_unit == LineTypesNS::Unit::In)
                    result = result * 0.039370f;
                len += result;
            }
        }
        QString unit_str;
        int little_len = 0;
        if(_line_unit == LineTypesNS::Unit::Cm)
        {
            unit_str= "cm";
            little_len = 2;
        }
        else if(_line_unit == LineTypesNS::Unit::In)
        {
            unit_str= "in";
            little_len = 3;
        }
        else if(_line_unit == LineTypesNS::Unit::Mm)
        {
            unit_str = "mm";
            little_len = 1;
        }
        else if(_line_unit == LineTypesNS::Unit::Px)
        {
            unit_str = "px";
            little_len = 0;
        }
        else
        {
            unit_str = "error";
            little_len = 4;
        }
        QString ret = QString::number(len,'f',little_len)+QString("(%1)").arg(unit_str) ;
        return ret;
    }
    else if(line.line_type == LineTypesNS::Type::Angle_Line)
    {
        float angle =0;
        int size = points.size();

        if(size >=3)
        {
            const float M_PI =3.14159265;
            auto point_1 = points.at(size -3);
            auto point_2 = points.at(size -2);
            auto point_3 = points.at(size -1);
            double theta = atan2(point_2.x - point_1.x, point_2.y - point_1.y) - atan2(point_2.x - point_3.x, point_2.y - point_3.y);
            if (theta > M_PI)
                theta -= 2 * M_PI;
            if (theta < -M_PI)
                theta += 2 * M_PI;
            angle = abs(theta * 180.0 / M_PI);
        }
        QString ret = QString::number(angle,'f',2)+QString("°") ;
        return ret;
    }
    else
        return "NAN";
}



void LineManager::fix(qint16 & x, qint16 & y)
{
    if(_need_fix)
    {//可能要改成4个旋转角度+和2种状态的8种情况进行计算
        //把旧点坐标系转成当前鼠标的坐标系，然后和鼠标点击位置进行对比
        if(_hor_mirror)
        {
            y = _current_picture_height-y;
        }
        if(_ver_mirror)
        {
            x = _current_picture_width-x;
        }
    }
    else //不需要修复
    {
        if(_hor_mirror)
        {
            x = _current_picture_width-x;
        }
        if(_ver_mirror)
        {
            y = _current_picture_height-y;
        }
    }
}

QString LineManager::getCurrent_angle_color() const
{
    return _current_angle_color;
}

void LineManager::setCurrent_angle_color(const QString &current_angle_color)
{
    if(_current_angle_color != current_angle_color)
    {
        QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat);
        settings.setValue("_current_angle_color",current_angle_color);
        _current_angle_color = current_angle_color;
        if(_mouse_task == MouseTask::NewLine && _line_task == LineTask::Angle && _current_task_line != nullptr)
        {
            _current_task_line->line_color = _current_angle_color;
            repaint_checked_line(_current_picture_id);
        }
        current_angle_colorChanged();
    }
}

QString LineManager::getCurrent_pen_color() const
{
    return _current_pen_color;
}

void LineManager::setCurrent_pen_color(const QString &current_pen_color)
{
    if(_current_pen_color != current_pen_color)
    {
        QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat);
        settings.setValue("_current_pen_color", current_pen_color);
        _current_pen_color = current_pen_color;
        if(_mouse_task == MouseTask::NewLine && _line_task == LineTask::Pen && _current_task_line != nullptr)
        {
            _current_task_line->line_color = _current_pen_color;
            repaint_checked_line(_current_picture_id);
        }
        current_pen_colorChanged();
    }
}

void LineManager::getPainterPath(QPainterPath &painter_path)
{
    painter_path = _cur_painter_path;
}

std::shared_ptr<LineTypesNS::Line> LineManager::getCurrentTaskPen()
{
    return _current_task_pen;
}

bool LineManager::saveNewPenData()
{
    if (_current_task_pen->line_data.size() > 2) {
        _current_task_pen->line_data.pop_back();
        bool ret = LineTable::getInstance()->add(*_current_task_pen); //存到数据库
        QJsonObject info = line_to_json(*_current_task_pen);
        setChecked_line_id(_current_task_pen->line_id);
        return ret;
    } else {
        return  false;
    }
}

bool LineManager::getLine_undo()
{
    return _line_undo;
}

bool LineManager::getLine_redo()
{
    return  _line_redo;
}

void LineManager::setLine_undo(bool undo)
{
    if (undo != _line_undo) {
        _line_undo = undo;
        emit line_undoChanged();
    }
}

void LineManager::setLine_redo(bool redo)
{
    if (redo != _line_redo) {
        _line_redo = redo;
        emit line_redoChanged();
    }
}

QString LineManager::getCurrent_line_color() const
{
    return _current_line_color;
}

void LineManager::setCurrent_line_color(const QString &current_line_color)
{
    if(_current_line_color != current_line_color)
    {
        QSettings settings(Tools::get_ini_file_name(),QSettings::IniFormat);
        settings.setValue("_current_line_color",current_line_color);
        _current_line_color = current_line_color;
        if(_mouse_task == MouseTask::NewLine && _line_task == LineTask::Line && _current_task_line != nullptr)
        {
            _current_task_line->line_color = _current_line_color;
            repaint_checked_line(_current_picture_id);
        }

        current_line_colorChanged();
    }
}

qint64 LineManager::getChecked_line_id() const
{
    return _checked_line_id;
}

void LineManager::setChecked_line_id(const qint64 &checked_line_id)
{
    if(_checked_line_id != checked_line_id)
    {
        _checked_line_id = checked_line_id;
        emit checked_line_idChanged();
    }
}

void LineManager::remove_this_patient_all_lines(qint64 patient_id)
{
    if(_current_patient_id == patient_id)
    {
        LineTable::getInstance()->remove_this_patient_all_lines(patient_id);
        _all_lines.clear();
        _current_picture_lines = nullptr;
        _current_patient_id= -1;
        _current_picture_id = "-1";
        _current_task_line = nullptr;
        _current_clicked_index_pos = -1;
        setLine_task(int(LineTask::NoLineTask));
        setMouse_task(int(MouseTask::NoMouseTask));
    }
}

float LineManager::getVer_per_pixel_mm() const
{
    return _ver_per_pixel_mm;
}

void LineManager::setVer_per_pixel_mm(float ver_per_pixel_mm)
{
    if(_ver_per_pixel_mm != ver_per_pixel_mm)
    {
        _ver_per_pixel_mm = ver_per_pixel_mm;
        emit ver_per_pixel_mmChanged();
    }
}

void LineManager::update_all_line_display_text()
{
    if(_current_picture_lines != nullptr)
    {
        for(auto it = _current_picture_lines->begin(); it != _current_picture_lines->end(); it++)
        {
            QString text = get_line_display_text(*(*it));
            emit line_display_text_changed((*it)->line_id,text);
        }
    }
    if(_current_picture_id != "-1")
        repaint_checked_line(_current_picture_id);
}

float LineManager::getHor_per_pixel_mm() const
{
    return _hor_per_pixel_mm;
}

void LineManager::setHor_per_pixel_mm(float hor_per_pixel_mm)
{
    if(_hor_per_pixel_mm != hor_per_pixel_mm)
    {
        _hor_per_pixel_mm = hor_per_pixel_mm;
        emit hor_per_pixel_mmChanged();
    }
}

bool LineManager::getShow_rule() const
{
    return _show_rule;
}

void LineManager::setShow_rule(bool show_rule)
{
    if(show_rule != _show_rule)
    {
        _show_rule = show_rule;
        emit show_ruleChanged();
    }
}

int LineManager::getLine_unit() const
{
    return int(_line_unit);
}

void LineManager::setLine_unit(int  line_unit)
{
    if(int(_line_unit) != line_unit)
    {
        _line_unit = LineTypesNS::Unit(line_unit);
        emit line_unitChanged();
    }
}

qint16 LineManager::getCurrent_picture_height() const
{
    return _current_picture_height;
}

void LineManager::setCurrent_picture_height(const qint16 &current_picture_height)
{
    if(_current_picture_height != current_picture_height)
    {
        _current_picture_height = current_picture_height;
        emit current_picture_heightChanged();
    }
}

qint16 LineManager::getCurrent_picture_width() const
{
    return _current_picture_width;
}

void LineManager::setCurrent_picture_width(const qint16 &current_picture_width)
{
    if(_current_picture_width != current_picture_width)
    {
        _current_picture_width = current_picture_width;
        emit current_picture_widthChanged();
    }
}

int LineManager::getRotate_factor() const
{
    return _rotate_factor;
}

void LineManager::setRotate_factor(int rotate_factor)
{
    if(_rotate_factor != rotate_factor)
    {
        _rotate_factor = rotate_factor;
        int ro = _rotate_factor < 0 ?  -_rotate_factor : _rotate_factor;
        _need_fix = ro  % 180 != 0; //需要修复
        emit rotate_factorChanged();
    }
    else
    {
        int ro = _rotate_factor < 0 ?  -_rotate_factor : _rotate_factor;
        _need_fix = ro  % 180 != 0; //需要修复
    }
}

bool LineManager::getVer_mirror() const
{
    return _ver_mirror;
}

void LineManager::setVer_mirror(bool ver_mirror)
{
    if(_ver_mirror != ver_mirror)
    {
        _ver_mirror = ver_mirror;
        emit ver_mirrorChanged();
    }
}

float LineManager::getScale_factor() const
{
    return _scale_factor;
}

void LineManager::setScale_factor(float scale_factor)
{
    if(_scale_factor != scale_factor)
    {
        _scale_factor = scale_factor;
        emit scale_factorChanged();
    }
}

bool LineManager::getHor_mirror() const
{
    return _hor_mirror;
}

void LineManager::setHor_mirror(bool hor_mirror)
{
    if(_hor_mirror != hor_mirror)
    {
        _hor_mirror = hor_mirror;
        emit hor_mirrorChanged();
    }
}

bool LineManager::getShow_cross_mouse() const
{
    return _show_cross_mouse;
}

void LineManager::setShow_cross_mouse(bool show_cross_mouse)
{
    if(_show_cross_mouse != show_cross_mouse)
    {
        _show_cross_mouse = show_cross_mouse;
        emit show_cross_mouseChanged();
    }
}

bool LineManager::end_new_line()
{ //存到数据库

    if(_current_task_line != nullptr)
    {
        if(_current_task_line->line_data.size() > 2)
        {
            _current_task_line->line_data.pop_back();
            LineTable::getInstance()->add(*_current_task_line); //存到数据库
            QJsonObject info = line_to_json(*_current_task_line);
            emit new_line(info);
            setChecked_line_id(_current_task_line->line_id);
        }
        else
        {
            for(auto it = _current_picture_lines->begin(); it != _current_picture_lines->end();it++)
            {
                if((*it)->line_id == _current_task_line->line_id)
                {
                    //撤销功能 有删除,去掉可以反撤销部分
                    while (_undo_index < _undo_list.count() - 1) {
                        if (_undo_list.last() == Operate::Delete) {
                            _current_undo_lines->erase(--_current_undo_lines->end());
                        }
                        _undo_list.removeLast();
                    }
                    _undo_list.append(Operate::Delete);
                    ++_undo_index;
                    setLine_redo(false);
                    setLine_undo(true);
                    _current_undo_lines->push_back(*it);

                    _current_picture_lines->erase(it);//删除线
                    break;
                }
            }
            Log::i(TAG,"点太少，取消保存到数据库");
            _current_task_line = nullptr;
            emit setChecked_line_id(-1);
            _current_clicked_index_pos = -1;//当点击到某条的点时点的索引，画线，和修改点时会频繁使用此参数
            _old_line_moving_pos={0,0};
        }

        return true;
    }
    else
    {
        Log::e(TAG,"代码逻辑错误,接收画线时，线为空");
        return false;
    }
}

void LineManager::end_change_point()
{
    LineTable::getInstance()->update_line_data(_current_task_line->line_id,_current_task_line->line_data);
    _current_clicked_index_pos = -1;//当点击到某条的点时点的索引，画线，和修改点时会频繁使用此参数
    setMouse_task(int(MouseTask::NoMouseTask));
    setLine_task(int(LineTask::NoLineTask));
    _old_line_moving_pos={0,0};
    emit line_display_text_changed(_current_task_line->line_id,get_line_display_text(*_current_task_line));
}

void LineManager::end_move_line()
{
    LineTable::getInstance()->update_line_data(_current_task_line->line_id,_current_task_line->line_data);
    _current_clicked_index_pos = -1;//当点击到某条的点时点的索引，画线，和修改点时会频繁使用此参数
    setMouse_task(int(MouseTask::NoMouseTask));
    setLine_task(int(LineTask::NoLineTask));
    _old_line_moving_pos={0,0};
    emit line_display_text_changed(_current_task_line->line_id,get_line_display_text(*_current_task_line));
}

void LineManager::empty_checked_line()
{
    _current_task_line = nullptr;
    emit setChecked_line_id(-1);
    _current_clicked_index_pos = -1;//当点击到某条的点时点的索引，画线，和修改点时会频繁使用此参数
    _old_line_moving_pos={0,0};
}

QJsonArray LineManager::get_lines()
{
    QJsonArray ret;
    if(_current_picture_lines != nullptr)
    {
        for(auto it = _current_picture_lines->begin(); it != _current_picture_lines->end();it++)
        {
            ret.insert(0,line_to_json(*(*it)));
        }
    }
    else
    {
        Log::i(TAG,"当前线为空");
    }
    return ret;
}

void LineManager::switch_this_line_id(qint64 id)
{
    if(id != _checked_line_id)
    {
        bool found = false;
        for(auto it = _current_picture_lines->begin(); it != _current_picture_lines->end();it++)
        {

            if((*it)->line_id == id)
            {
                _current_task_line = (*it);
                setChecked_line_id(id);
                emit repaint_checked_line(_current_picture_id);
                found = true;
                break;
            }
        }
        if(found == false)
        {
            _current_task_line = nullptr;
            setChecked_line_id(-1);
            emit repaint_checked_line(_current_picture_id);
        }
    }
}

void LineManager::delete_one_line(qint64 id)
{
    if(_current_picture_lines != nullptr)
        for(auto it = _current_picture_lines->begin(); it != _current_picture_lines->end();it++)
        {
            if((*it)->line_id == id)
            {
                //撤销功能 有删除,去掉可以反撤销部分
                while (_undo_index < _undo_list.count() - 1) {
                    if (_undo_list.last() == Operate::Delete) {
                        _current_undo_lines->erase(--_current_undo_lines->end());
                    }
                    _undo_list.removeLast();
                }
                _undo_list.append(Operate::Delete);
                ++_undo_index;
                _current_undo_lines->push_back(*it);
                setLine_redo(false);
                setLine_undo(true);

                _current_picture_lines->erase(it);//删除线
                if(_checked_line_id == id)
                {
                    setChecked_line_id(-1);
                    _current_task_line = nullptr;
                }
                LineTable::getInstance()->remove(id);
                emit repaint_checked_line(_current_picture_id); //渲染
                break;
            }
        }
}

void LineManager::update_line_color(qint64 id, const QString &new_color)
{
    if(_current_picture_lines != nullptr)
    {
        for(auto it = _current_picture_lines->begin(); it != _current_picture_lines->end(); it++)
        {
            if((*it)->line_id == id)
            {
                LineTable::getInstance()->update_color(id,new_color);
                (*it)->line_color = new_color;
                emit repaint_checked_line(_current_picture_id);
                break;
            }
        }

    }
}

//撤销
void LineManager::undo()
{
    if (_undo_index < 0 || _undo_index > _undo_list.count() - 1 || _undo_list.count() == 0) {
        return;
    }

    switch (_undo_list[_undo_index]) {
        case  Operate::Delete: {
            auto it = --_current_undo_lines->end();
            _current_picture_lines->push_back(*it);
            _current_undo_lines->erase(it);
            LineTable::getInstance()->add(*it->get());//新增到数据库
            --_undo_index;
            break;
        }
        case  Operate::Insert: {
            auto it = --_current_picture_lines->end();
            qint64 id = (*it)->line_id; // 获取线id
            _current_undo_lines->push_back(*it);
            _current_picture_lines->erase(it);//删除线
            LineTable::getInstance()->remove(id);//从数据库中删除

            --_undo_index;
            break;
        }
        default:
            break;
    }
    setLine_redo(true);
    if (_undo_index < 0) {
        setLine_undo(false);
    }
    emit repaint_checked_line(_current_picture_id); //渲染
//    qDebug() << "0 undo_index == " << _undo_index << " undo_list_count == " << _undo_list.count() << " _current_picture_lines == "
//             << _current_picture_lines->size() << " _current_undo_lines == " << _current_undo_lines->size();
}
//反撤销
void LineManager::reverseUndo()
{
    int index = _undo_index + 1;
    if (index < 0 || index > _undo_list.count() - 1 || _undo_list.count() == 0) {
        return;
    }
    ++_undo_index;
    switch (_undo_list[_undo_index]) {
        case  Operate::Delete: {
            auto it = --_current_picture_lines->end();
            qint64 id = (*it)->line_id; // 获取线id
            _current_undo_lines->push_back(*it);
            _current_picture_lines->erase(it);//删除线
            LineTable::getInstance()->remove(id);//从数据库中删除
           // emit repaint_checked_line(_current_picture_id); //渲染
            break;
        }
        case  Operate::Insert: {
            auto it = --_current_undo_lines->end();
            _current_picture_lines->push_back(*it);
            _current_undo_lines->erase(it);//删除线
            LineTable::getInstance()->add(*it->get());//新增到数据库
            break;
        }
        default:
            break;
    }
    setLine_undo(true);
    if (_undo_index + 1 > _undo_list.count() - 1) {
        setLine_redo(false);
    }
    emit repaint_checked_line(_current_picture_id); //渲染
//    qDebug() << "1 undo_index == " << _undo_index << " undo_list_count == " << _undo_list.count() << " _current_picture_lines == "
//             << _current_picture_lines->size() << " _current_undo_lines == " << _current_undo_lines->size();

}

void LineManager::clearAll()
{

    if(_current_picture_lines != nullptr)
    {
    for (auto it = _current_picture_lines->begin(); it != _current_picture_lines->end(); ++it) {
        qint64 id = (*it)->line_id; // 获取线id
        _current_picture_lines->erase(it);//删除线
        LineTable::getInstance()->remove(id);//从数据库中删除
    }
    _undo_list.clear();
    _undo_index = -1;
    setLine_undo(false);
    setLine_redo(false);
    _current_undo_lines = std::shared_ptr<std::list<std::shared_ptr<LineTypesNS::Line>>>(new std::list<std::shared_ptr<LineTypesNS::Line>>());
    repaint_checked_line(_current_picture_id);
    }
    }

LineManager *LineManager::get_instance()
{
    if(_single_instace == nullptr)
        _single_instace = new LineManager(nullptr);
    return _single_instace;
}

void LineManager::load_patien_all_picture_lines(qint64 patient_id)
{
    if(_current_patient_id != patient_id)
    {
        _current_picture_id = "-1";
        _current_patient_id = patient_id;
        LineTable::getInstance()->query_this_patient_lines(patient_id, _all_lines);
    }
}

void LineManager::checked_to_picture(const QString &picture_id)
{
    if(picture_id == "-1")
    {
        if(_current_picture_id == "-1")
            return;
        else
        {
            if(_current_task_line != nullptr)
            {
                _current_task_line  = nullptr;
            }
            emit repaint_checked_line(_current_picture_id);
            _current_picture_id = picture_id;
        }
    }
    else
    {
        if(_current_picture_id == picture_id)
            return;
        else
        {
            if(_current_task_line != nullptr)
                _current_task_line  = nullptr;
            emit repaint_checked_line(_current_picture_id);
            _current_picture_id = picture_id;
            auto this_pictrue_all_line_it = _all_lines.find(picture_id);
            if(this_pictrue_all_line_it == _all_lines.end())
            {
                Log::i(TAG,"图片id找不到线集合,创建一个");
                auto temp   = std::shared_ptr<std::list<std::shared_ptr<LineTypesNS::Line>>>(new std::list<std::shared_ptr<LineTypesNS::Line>>());
                _all_lines[picture_id] = temp;
                _current_picture_lines = temp;
            }
            else
                _current_picture_lines = this_pictrue_all_line_it->second;
        }
        _undo_index = -1;
        _undo_list.clear();
        _current_undo_lines = std::shared_ptr<std::list<std::shared_ptr<LineTypesNS::Line>>>(new std::list<std::shared_ptr<LineTypesNS::Line>>());
        for (auto it = _current_picture_lines->begin(); it !=  _current_picture_lines->end(); ++it) {
            _undo_list.append(Operate::Insert);
            ++_undo_index;
        }
        setLine_redo(false);
        setLine_undo(true);
    }
}

std::shared_ptr<std::list<std::shared_ptr<LineTypesNS::Line> > > LineManager::get_lines(const QString &picture_id)
{
    auto it = _all_lines.find(picture_id);
    if(it == _all_lines.end())
        return nullptr;
    else
        return it->second;
}

int LineManager::getLine_task() const
{
    return int(_line_task);
}

void LineManager::setLine_task(int line_task)
{
    if(int(_line_task) != line_task)
    {
        _line_task = LineTask(line_task);
        emit line_taskChanged();
    }
}

int LineManager::getMouse_task() const
{
    return int(_mouse_task);
}

void LineManager::setMouse_task(int mouse_task)
{

    if(int(_mouse_task) != mouse_task)
    {
        _mouse_task = MouseTask(mouse_task);
        emit mouse_taskChanged();
    };
}

std::shared_ptr<LineTypesNS::Line> LineManager::get_current_task_line()
{
    return _current_task_line;
}




