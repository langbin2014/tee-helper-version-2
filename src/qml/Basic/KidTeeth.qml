﻿import QtQuick 2.0
import QtQuick.Controls 2.0
Item {
    id:root
    readonly property int row_width: 390
    readonly  property int row_height: 65
    property real _scale_factor : 1.0
    signal model_checked_ele_changed(var checked_indexs, var checked_texts) //用户点击牙齿按钮的时候发出
    signal sigSetTeethChanged(var tooth_form) //设置牙齿的文本
    onSigSetTeethChanged: {
        switch (String(tooth_form)){
        case 'Palmer':
            var top1=["E","D","C","B","A","A","B","C","D","E"]
            var bottom1=["E","D","C","B","A","A","B","C","D","E"]
            for(let i =0; i < model_top.count; i++)
            {
                model_top.setProperty(i,"_text",top1[i])
                model_right_top.setProperty(i,"_text",top1[i])
            }
            for(let i1 =0; i1 < model_bottom.count; i1++)
            {
                model_bottom.setProperty(i1,"_text",bottom1[i1])
                model_right_bottom.setProperty(i1,"_text",bottom1[i1])
            }
            ;
            break;
        case 'UNS':
            var top2=["A","B","C","D","E","F","G","H","I","J"]
            var bottom2=["T","S","R","Q","P","O","N","M","L","K"]
            for(let j =0; j < model_top.count; j++)
            {
                model_top.setProperty(j,"_text",top2[j])
                model_right_top.setProperty(j,"_text",top2[j])
            }
            for(let i2 =0; i2 < model_bottom.count; i2++)
            {
                model_bottom.setProperty(i2,"_text",bottom2[i2])
                model_right_bottom.setProperty(i2,"_text",bottom2[i2])
            }
            ;
            break;
        case 'FDI':
            var top3=["55","54","53","52","51","61","62","63","64","65"]
            var bottom3=["85","84","83","82","81","71","72","73","74","75"]
            for(let k =0; k < model_top.count; k++)
            {
                model_top.setProperty(k,"_text",top3[k])
                model_right_top.setProperty(k,"_text",top3[k])
            }
            for(let i3 =0; i3 < model_bottom.count; i3++)
            {
                model_bottom.setProperty(i3,"_text",bottom3[i3])
                model_right_bottom.setProperty(i3,"_text",bottom3[i3])
            }
            ;
            break;
        default :
            break;
        }
    }
    function set_checked_indexs(checked_indexs,checked_texts)
    {//先重置再初始化
        for(let i =0; i < model_top.count; i++)
        {
            model_top.setProperty(i,"_checked",false)
            model_right_top.setProperty(i,"_checked",false)
        }

        for(let i =0; i < model_bottom.count; i++)
        {
            model_bottom.setProperty(i,"_checked",false)
            model_right_bottom.setProperty(i,"_checked",false)
        }

        for(let i =0 ; i< checked_indexs.length;i++)
        {
            let ele = checked_indexs[i]
            if(ele  < model_top.count){
                var a = model_top.get(ele)
                for(var j = 0 ; j < checked_texts.length; j++){
                    let l = checked_texts[j]
                    if(a._text === l){
                        model_top.setProperty(ele,"_checked",true)
                        model_right_top.setProperty(ele,"_checked",true)
                    }
                }
            }
            else{
                var a1 = model_bottom.get(ele - model_top.count)
                for(var j1 = 0 ; j1 < checked_texts.length; j1++){
                    let l1 = checked_texts[j1]
                    if(a1._text === l1){
                        model_bottom.setProperty(ele - model_top.count,"_checked",true)
                        model_right_bottom.setProperty(ele - model_top.count,"_checked",true)
                    }
                }

            }
        }
    }

    function get_checked_indexs()
    {
        let result = []
        for(let i =0; i < model_top.count; i++)
        {
            let ele = model_top.get(i)
            if(ele._checked ===  true)
                result.push(i)
        }
        for(let i =0; i < model_bottom.count; i++)
        {
            let ele = model_bottom.get(i)
            if(ele._checked ===  true)
                result.push(i+model_top.count)
        }
        return result
    }
    function get_checked_texts()
    {
        let result = []
        for(let i =0; i < model_top.count; i++)
        {
            let ele = model_top.get(i)
            if(ele._checked ===  true)

                result.push(ele._text)
        }

        for(let i =0; i < model_bottom.count; i++)
        {
            let ele = model_bottom.get(i)
            if(ele._checked ===  true)
                result.push(ele._text)
        }
        return result
    }
    function reset_width_height(){
        if(width <10 || height < 10)
            return
        let normal_width = row_width
        let normal_height = row_height*2 + 14
        let w = width
        let h = height
        let bi_li_1 = w/normal_width
        let bi_li_2 = h/normal_height
        let min = Math.min(bi_li_1,bi_li_2)
//        if(min > 1.5)
            min = 1.5
        _scale_factor = min * 0.7
    }


    function setRightColumnVisible(is_visible) {
        right_column.visible = false//is_visible
    }

    onWidthChanged: reset_width_height()
    onHeightChanged: reset_width_height()
    Component.onCompleted:{

        reset_width_height()
    }
//    Connections {
//        //qml 连接 c++ 信号
//        target:setting_manage
//        onSigSetTeethForm:{
//            sigSetTeethChanged(setting_manage.getTeethForm())
//        }
//    }
onVisibleChanged: {
    //sigSetTeethChanged(setting_manage.getTeethForm())
}
    Row {
        id:row1
        anchors.centerIn: parent
        spacing: 10
        Column{
    //        anchors.centerIn: parent
            spacing: 14 * _scale_factor
            Row{
                anchors.horizontalCenter: parent.horizontalCenter
                id:row_top
                width: row_width * _scale_factor
                height: row_height * _scale_factor
                spacing: 4
                Repeater{
                    model: ListModel{
                        id:model_top
                        ListElement{
                            _text:"E"
                            _not_checked_url:"qrc:/images/kid/nochecked/top/left/e.png"
                            _checked_url:"qrc:/images/kid/checked/top/left/e.png"
                            _rotation:0
                            _bottom_margin:0
                            _scale:1.0
                            _checked:false
                            _cur_index: 0
                        }

                        ListElement{
                            _text:"D"
                            _not_checked_url:"qrc:/images/kid/nochecked/top/left/d.png"
                            _checked_url:"qrc:/images/kid/checked/top/left/d.png"
                            _rotation:0
                            _bottom_margin:-10
                            _scale:1.0
                            _checked:false
                            _cur_index: 1
                        }

                        ListElement{
                            _text:"C"
                            _not_checked_url:"qrc:/images/kid/nochecked/top/left/c.png"
                            _checked_url:"qrc:/images/kid/checked/top/left/c.png"
                            _rotation:0
                            _bottom_margin:-10
                            _scale:1.0
                            _checked:false
                            _cur_index: 2
                        }
                        ListElement{
                            _text:"B"
                            _not_checked_url:"qrc:/images/kid/nochecked/top/left/b.png"
                            _checked_url:"qrc:/images/kid/checked/top/left/b.png"
                            _rotation:0
                            _bottom_margin:-6
                            _scale:1.0
                            _checked:false
                            _cur_index: 3
                        }
                        ListElement{
                            _text:"A"
                            _not_checked_url:"qrc:/images/kid/nochecked/top/left/a.png"
                            _checked_url:"qrc:/images/kid/checked/top/left/a.png"
                            _rotation:0
                            _bottom_margin:-6
                            _scale:1.0
                            _checked:false
                            _cur_index: 4
                        }
                        ListElement{
                            _text:"A"
                            _not_checked_url:"qrc:/images/kid/nochecked/top/right/a.png"
                            _checked_url:"qrc:/images/kid/checked/top/right/a.png"
                            _rotation:0
                            _bottom_margin:-6
                            _scale:1.0
                            _checked:false
                            _cur_index: 5
                        }
                        ListElement{
                            _text:"B"
                            _not_checked_url: "qrc:/images/kid/nochecked/top/right/b.png"
                            _checked_url:"qrc:/images/kid/checked/top/right/b.png"
                            _rotation:0
                            _bottom_margin:-6
                            _scale:1.0
                            _checked:false
                            _cur_index: 6
                        }
                        ListElement{
                            _text:"C"
                            _not_checked_url:"qrc:/images/kid/nochecked/top/right/c.png"
                            _checked_url:"qrc:/images/kid/checked/top/right/c.png"
                            _rotation:0
                            _bottom_margin:-10
                            _scale:1.0
                            _checked:false
                            _cur_index: 7
                        }
                        ListElement{
                            _text:"D"
                            _not_checked_url:"qrc:/images/kid/nochecked/top/right/d.png"
                            _checked_url:"qrc:/images/kid/checked/top/right/d.png"
                            _rotation:0
                            _bottom_margin:-10
                            _scale:1.0
                            _checked:false
                            _cur_index: 8
                        }
                        ListElement{
                            _text:"E"
                            _not_checked_url:"qrc:/images/kid/nochecked/top/right/e.png"
                            _checked_url:"qrc:/images/kid/checked/top/right/e.png"
                            _rotation:0
                            _bottom_margin:0
                            _scale:1.0
                            _checked:false
                            _cur_index: 9
                        }
                    }


                   Item{
                        width: (row_top.width-9*row_top.spacing)/10
                        height: row_top.height
                        Text {
                            anchors.top: parent.top
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: _text
                            color: "white"
                            font.bold: true
                            font.pixelSize: 20 * _scale_factor
                        }
                        Image{
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: _bottom_margin * _scale_factor
                            anchors.horizontalCenter: parent.horizontalCenter
                            width: sourceSize.width * _scale * _scale_factor
                            height: sourceSize.height * _scale * _scale_factor
                            source: _checked ? _checked_url  :  _not_checked_url
                            rotation: _rotation
                            MouseArea{
                                anchors.fill: parent
                                onClicked: {

                                    model_right_top.setProperty(_cur_index, "_checked", !_checked)

//                                    _checked = !_checked
//                                    model_checked_ele_changed(get_checked_indexs(),get_checked_texts())
                                }
                            }
                        }

                    }
                }
            }
            Row{
                anchors.horizontalCenter: parent.horizontalCenter
                id:bottom_row
                width: row_width * _scale_factor
                height: row_height * _scale_factor
                spacing: 4
                Repeater{
                    model: ListModel{
                        id:model_bottom
                        ListElement{
                            _text:"E"
                            _not_checked_url:"qrc:/images/kid/nochecked/bottom/left/e.png"
                            _checked_url:"qrc:/images/kid/checked/bottom/left/e.png"
                            _rotation:0
                            _top_margin:-10
                            _scale:1.0
                            _checked:false
                            _cur_index: 0
                        }
                        ListElement{
                            _text:"D"
                            _not_checked_url:"qrc:/images/kid/nochecked/bottom/left/d.png"
                            _checked_url:"qrc:/images/kid/checked/bottom/left/d.png"
                            _rotation:0
                            _top_margin:0
                            _scale:1.0
                            _checked:false
                            _cur_index: 1
                        }

                        ListElement{
                            _text:"C"
                            _not_checked_url:"qrc:/images/kid/nochecked/bottom/left/c.png"
                            _checked_url:"qrc:/images/kid/checked/bottom/left/c.png"
                            _rotation:0
                            _top_margin:6
                            _scale:1.0
                            _checked:false
                            _cur_index: 2
                        }
                        ListElement{
                            _text:"B"
                            _not_checked_url:"qrc:/images/kid/nochecked/bottom/left/b.png"
                            _checked_url:"qrc:/images/kid/checked/bottom/left/b.png"
                            _rotation:0
                            _top_margin:0
                            _scale:1.0
                            _checked:false
                            _cur_index: 3
                        }
                        ListElement{
                            _text:"A"
                            _not_checked_url:"qrc:/images/kid/nochecked/bottom/left/a.png"
                            _checked_url:"qrc:/images/kid/checked/bottom/left/a.png"
                            _rotation:0
                            _top_margin:0
                            _scale:1.0
                            _checked:false
                            _cur_index: 4
                        }
                        ListElement{
                            _text:"A"
                            _not_checked_url:"qrc:/images/kid/nochecked/bottom/right/a.png"
                            _checked_url:"qrc:/images/kid/checked/bottom/right/a.png"
                            _rotation:0
                            _top_margin:0
                            _scale:1.0
                            _checked:false
                            _cur_index: 5
                        }
                        ListElement{
                            _text:"B"
                            _not_checked_url: "qrc:/images/kid/nochecked/bottom/right/b.png"
                            _checked_url:"qrc:/images/kid/checked/bottom/right/b.png"
                            _rotation:0
                            _top_margin:0
                            _scale:1.0
                            _checked:false
                            _cur_index: 6
                        }
                        ListElement{
                            _text:"C"
                            _not_checked_url:"qrc:/images/kid/nochecked/bottom/right/c.png"
                            _checked_url:"qrc:/images/kid/checked/bottom/right/c.png"
                            _rotation:0
                            _top_margin:6
                            _scale:1.0
                            _checked:false
                            _cur_index: 7
                        }
                        ListElement{
                            _text:"D"
                            _not_checked_url:"qrc:/images/kid/nochecked/bottom/right/d.png"
                            _checked_url:"qrc:/images/kid/checked/bottom/right/d.png"
                            _rotation:0
                            _top_margin:0
                            _scale:1.0
                            _checked:false
                            _cur_index: 8
                        }
                        ListElement{
                            _text:"E"
                            _not_checked_url:"qrc:/images/kid/nochecked/bottom/right/e.png"
                            _checked_url:"qrc:/images/kid/checked/bottom/right/e.png"
                            _rotation:0
                            _top_margin:-10
                            _scale:1.0
                            _checked:false
                            _cur_index: 9
                        }
                    }


                    Item{
                        width: (bottom_row.width-9*bottom_row.spacing)/10
                        height: bottom_row.height
                        Text {
                            anchors.bottom: parent.bottom
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: _text
                            font.bold: true
                            font.pixelSize: 20 * _scale_factor
                            color: "white"
                        }
                        Image{
                            anchors.top: parent.top
                            anchors.topMargin: _top_margin * _scale_factor
                            anchors.horizontalCenter: parent.horizontalCenter
                            width: sourceSize.width * _scale * _scale_factor
                            height: sourceSize.height * _scale *_scale_factor
                            source: _checked ? _checked_url  :  _not_checked_url
                            rotation: _rotation
                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    model_right_bottom.setProperty(_cur_index, "_checked", !_checked)

//                                    _checked = !_checked
//                                    model_checked_ele_changed(get_checked_indexs(),get_checked_texts())
                                }
                            }
                        }
                    }
                }
            }
        }
        Column{
    //        anchors.centerIn: parent
            id: right_column
            visible: false
            spacing: 14 * _scale_factor
            Row{
                anchors.horizontalCenter: parent.horizontalCenter
                id:row_right_top
                width: row_width * _scale_factor
                height: row_height * _scale_factor
                spacing: 4
                Repeater{
                    model: ListModel{
                        id:model_right_top
                        ListElement{
                            _text:"E"
                            _not_checked_url:"qrc:/images/kid/nochecked/top/left/e.png"
                            _checked_url:"qrc:/images/kid/checked/top/left/e.png"
                            _rotation:0
                            _bottom_margin:0
                            _scale:1.0
                            _checked:false
                            _cur_index: 0
                        }

                        ListElement{
                            _text:"D"
                            _not_checked_url:"qrc:/images/kid/nochecked/top/left/d.png"
                            _checked_url:"qrc:/images/kid/checked/top/left/d.png"
                            _rotation:0
                            _bottom_margin:-10
                            _scale:1.0
                            _checked:false
                            _cur_index: 1
                        }

                        ListElement{
                            _text:"C"
                            _not_checked_url:"qrc:/images/kid/nochecked/top/left/c.png"
                            _checked_url:"qrc:/images/kid/checked/top/left/c.png"
                            _rotation:0
                            _bottom_margin:-10
                            _scale:1.0
                            _checked:false
                            _cur_index: 2
                        }
                        ListElement{
                            _text:"B"
                            _not_checked_url:"qrc:/images/kid/nochecked/top/left/b.png"
                            _checked_url:"qrc:/images/kid/checked/top/left/b.png"
                            _rotation:0
                            _bottom_margin:-6
                            _scale:1.0
                            _checked:false
                            _cur_index: 3
                        }
                        ListElement{
                            _text:"A"
                            _not_checked_url:"qrc:/images/kid/nochecked/top/left/a.png"
                            _checked_url:"qrc:/images/kid/checked/top/left/a.png"
                            _rotation:0
                            _bottom_margin:-6
                            _scale:1.0
                            _checked:false
                            _cur_index: 4
                        }
                        ListElement{
                            _text:"A"
                            _not_checked_url:"qrc:/images/kid/nochecked/top/right/a.png"
                            _checked_url:"qrc:/images/kid/checked/top/right/a.png"
                            _rotation:0
                            _bottom_margin:-6
                            _scale:1.0
                            _checked:false
                            _cur_index: 5
                        }
                        ListElement{
                            _text:"B"
                            _not_checked_url: "qrc:/images/kid/nochecked/top/right/b.png"
                            _checked_url:"qrc:/images/kid/checked/top/right/b.png"
                            _rotation:0
                            _bottom_margin:-6
                            _scale:1.0
                            _checked:false
                            _cur_index: 6
                        }
                        ListElement{
                            _text:"C"
                            _not_checked_url:"qrc:/images/kid/nochecked/top/right/c.png"
                            _checked_url:"qrc:/images/kid/checked/top/right/c.png"
                            _rotation:0
                            _bottom_margin:-10
                            _scale:1.0
                            _checked:false
                            _cur_index: 7
                        }
                        ListElement{
                            _text:"D"
                            _not_checked_url:"qrc:/images/kid/nochecked/top/right/d.png"
                            _checked_url:"qrc:/images/kid/checked/top/right/d.png"
                            _rotation:0
                            _bottom_margin:-10
                            _scale:1.0
                            _checked:false
                            _cur_index: 8
                        }
                        ListElement{
                            _text:"E"
                            _not_checked_url:"qrc:/images/kid/nochecked/top/right/e.png"
                            _checked_url:"qrc:/images/kid/checked/top/right/e.png"
                            _rotation:0
                            _bottom_margin:0
                            _scale:1.0
                            _checked:false
                            _cur_index: 9
                        }
                    }


                   Item{
                        width: (row_top.width-9*row_top.spacing)/10
                        height: row_top.height
                        Text {
                            anchors.top: parent.top
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: _text
                            color: "white"
                            font.bold: true
                            font.pixelSize: 20 * _scale_factor
                        }

                        CheckBox{
                            width: 25
                            height: 25
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: 0
                            anchors.horizontalCenter: parent.horizontalCenter
                            hoverEnabled: true
                            indicator: null
                            checked: _checked
                            background: Rectangle{
                                radius: 4
                                border.width: 2
                                border.color: parent.hovered || parent.checked ? _checked_or_content_background_color : "gray"
                                anchors.fill: parent
                                color: parent.checked ? _checked_or_content_background_color : "#fff"
                                Image {
                                    anchors.fill: parent
                                    anchors.margins: 2
                                    source: "qrc:/images/checked_icon.png"
                                    visible: parent.parent.checked
                                }
                            }
                            onCheckedChanged: {
                                _checked = checked
                                model_top.setProperty(_cur_index, "_checked", _checked)
                                var a = get_checked_indexs()
                                model_checked_ele_changed(get_checked_indexs(),get_checked_texts())
                            }
                        }
//                        Image{
//                            anchors.bottom: parent.bottom
//                            anchors.bottomMargin: _bottom_margin * _scale_factor
//                            anchors.horizontalCenter: parent.horizontalCenter
//                            width: sourceSize.width * _scale * _scale_factor
//                            height: sourceSize.height * _scale * _scale_factor
//                            source: _checked ? _checked_url  :  _not_checked_url
//                            rotation: _rotation
//                            MouseArea{
//                                anchors.fill: parent
//                                onClicked: {
//                                    _checked = !_checked
//                                    model_checked_ele_changed(get_checked_indexs(),get_checked_texts())
//                                }
//                            }
//                        }

                    }
                }
            }
            Row{
                anchors.horizontalCenter: parent.horizontalCenter
                id:bottom_right_row
                width: row_width * _scale_factor
                height: row_height * _scale_factor
                spacing: 4
                Repeater{
                    model: ListModel{
                        id:model_right_bottom
                        ListElement{
                            _text:"E"
                            _not_checked_url:"qrc:/images/kid/nochecked/bottom/left/e.png"
                            _checked_url:"qrc:/images/kid/checked/bottom/left/e.png"
                            _rotation:0
                            _top_margin:-10
                            _scale:1.0
                            _checked:false
                            _cur_index: 0
                        }
                        ListElement{
                            _text:"D"
                            _not_checked_url:"qrc:/images/kid/nochecked/bottom/left/d.png"
                            _checked_url:"qrc:/images/kid/checked/bottom/left/d.png"
                            _rotation:0
                            _top_margin:0
                            _scale:1.0
                            _checked:false
                            _cur_index: 1
                        }

                        ListElement{
                            _text:"C"
                            _not_checked_url:"qrc:/images/kid/nochecked/bottom/left/c.png"
                            _checked_url:"qrc:/images/kid/checked/bottom/left/c.png"
                            _rotation:0
                            _top_margin:6
                            _scale:1.0
                            _checked:false
                            _cur_index: 2
                        }
                        ListElement{
                            _text:"B"
                            _not_checked_url:"qrc:/images/kid/nochecked/bottom/left/b.png"
                            _checked_url:"qrc:/images/kid/checked/bottom/left/b.png"
                            _rotation:0
                            _top_margin:0
                            _scale:1.0
                            _checked:false
                            _cur_index: 3
                        }
                        ListElement{
                            _text:"A"
                            _not_checked_url:"qrc:/images/kid/nochecked/bottom/left/a.png"
                            _checked_url:"qrc:/images/kid/checked/bottom/left/a.png"
                            _rotation:0
                            _top_margin:0
                            _scale:1.0
                            _checked:false
                            _cur_index: 4
                        }
                        ListElement{
                            _text:"A"
                            _not_checked_url:"qrc:/images/kid/nochecked/bottom/right/a.png"
                            _checked_url:"qrc:/images/kid/checked/bottom/right/a.png"
                            _rotation:0
                            _top_margin:0
                            _scale:1.0
                            _checked:false
                            _cur_index: 5
                        }
                        ListElement{
                            _text:"B"
                            _not_checked_url: "qrc:/images/kid/nochecked/bottom/right/b.png"
                            _checked_url:"qrc:/images/kid/checked/bottom/right/b.png"
                            _rotation:0
                            _top_margin:0
                            _scale:1.0
                            _checked:false
                            _cur_index: 6
                        }
                        ListElement{
                            _text:"C"
                            _not_checked_url:"qrc:/images/kid/nochecked/bottom/right/c.png"
                            _checked_url:"qrc:/images/kid/checked/bottom/right/c.png"
                            _rotation:0
                            _top_margin:6
                            _scale:1.0
                            _checked:false
                            _cur_index: 7
                        }
                        ListElement{
                            _text:"D"
                            _not_checked_url:"qrc:/images/kid/nochecked/bottom/right/d.png"
                            _checked_url:"qrc:/images/kid/checked/bottom/right/d.png"
                            _rotation:0
                            _top_margin:0
                            _scale:1.0
                            _checked:false
                            _cur_index: 8
                        }
                        ListElement{
                            _text:"E"
                            _not_checked_url:"qrc:/images/kid/nochecked/bottom/right/e.png"
                            _checked_url:"qrc:/images/kid/checked/bottom/right/e.png"
                            _rotation:0
                            _top_margin:-10
                            _scale:1.0
                            _checked:false
                            _cur_index: 9
                        }
                    }


                    Item{
                        width: (bottom_row.width-9*bottom_row.spacing)/10
                        height: bottom_row.height
                        Text {
                            anchors.bottom: parent.bottom
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: _text
                            font.bold: true
                            font.pixelSize: 20 * _scale_factor
                            color: "white"
                        }

                        CheckBox{
                            width: 25
                            height: 25
                            anchors.top: parent.top
                            anchors.topMargin: 0
                            anchors.horizontalCenter: parent.horizontalCenter
                            hoverEnabled: true
                            indicator: null
                            checked: _checked

                            background: Rectangle{
                                radius: 4
                                border.width: 2
                                border.color: parent.hovered || parent.checked ? _checked_or_content_background_color : "gray"
                                anchors.fill: parent
                                color: parent.checked ? _checked_or_content_background_color : "#fff"
                                Image {
                                    anchors.fill: parent
                                    anchors.margins: 2
                                    source: "qrc:/images/checked_icon.png"
                                    visible: parent.parent.checked
                                }
                            }
                            onCheckedChanged: {
                                _checked = checked
                                model_bottom.setProperty(_cur_index, "_checked", _checked)
                                var a = get_checked_indexs()
                                model_checked_ele_changed(get_checked_indexs(),get_checked_texts())
                            }
                        }
//                        Image{
//                            anchors.top: parent.top
//                            anchors.topMargin: _top_margin * _scale_factor
//                            anchors.horizontalCenter: parent.horizontalCenter
//                            width: sourceSize.width * _scale * _scale_factor
//                            height: sourceSize.height * _scale *_scale_factor
//                            source: _checked ? _checked_url  :  _not_checked_url
//                            rotation: _rotation
//                            MouseArea{
//                                anchors.fill: parent
//                                onClicked: {
//                                    _checked = !_checked
//                                    model_checked_ele_changed(get_checked_indexs(),get_checked_texts())
//                                }
//                            }
//                        }
                    }
                }
            }
        }
    }
    Rectangle{
        height: row1.height
        width: 1
        anchors.centerIn: row1
        color:  "#ffffff"
    }
}
