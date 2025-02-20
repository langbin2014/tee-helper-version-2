import QtQuick 2.14
import QtQuick.Controls 2.14
import "qrc:/src/qml/Css/" as Css
Row {
    property alias label_text: text.text
    property alias label_width: text.width
    property alias label_height: text.height
    property alias label_color: text.color
    property alias combox_width: combox.width
    property alias combox_height: combox.height
    property alias combox_model: combox.model
    property alias combox_textRole: combox.textRole
    property alias input_text: input_text.text
    property alias combox_enable: combox.enabled
    property string key: ""
    property alias combox_index: combox.currentIndex
    property string control_type: "combox"
    property alias combox_displayText: combox.displayText
    property alias label_text_size: text.font.pixelSize
    property alias combox_text_size: combox.font.pixelSize
    property alias combox_editable: combox.editable
    property alias combox_edit_text: combox.editText
    property alias input_text_height: input_text.height
    property alias text_horizontalAlignment: text.horizontalAlignment
    property var font_pixelSize: 20
    property alias text_fontSizeMode: text.fontSizeMode
    signal current_Index
    spacing: 10

    Text {
        id: text
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 20;
        font.family: "微软雅黑"
        text: "text"
        width: 50
        height: 15
        color: "white"
    }

    ComboBox {
        id:combox
        property color _check_color
        property color _line_color
        property alias _popup_visible: combox.popup.visible
        /*显示的文字*/
        currentIndex: setting_manage.getIndex()
        font.pixelSize: font_pixelSize;
        font.family: "微软雅黑"
        width: 280; height: 40;
        opacity: pressed ? 0.5 :1.0
        editable: false
        enabled: false

        contentItem: TextField {
            id:input_text
            leftPadding: !combox.mirrored ? 12 : combox_editable && activeFocus ? 3 : 1;
            rightPadding: combox.mirrored ? 12 : combox_editable && activeFocus ? 3 : 1;
            topPadding: 6 - combox.padding;
            bottomPadding: 6 - combox.padding;

            text: combox.displayText

            enabled: combox_editable;
            autoScroll: combox_editable;
            readOnly: combox.down;
            inputMethodHints: combox.inputMethodHints;
            validator: combox.validator;
            anchors.horizontalCenter: combox.horizontalCenter
            anchors.horizontalCenterOffset:  -17
            anchors.left: combox.left
            font: combox.font;
            color: parent.hovered ?"white":"gray"
            selectionColor: combox.palette.highlight;
            selectedTextColor: combox.palette.highlightedText;
            verticalAlignment: Text.AlignVCenter;
            //renderType: Text.NativeRendering;
            background: Rectangle{color: "transparent"}


//            background: Rectangle {
//                visible: true//combox.enabled && combox_editable && !combox.flat;
//                border.width: parent && parent.activeFocus ? 2 : 1;
//                border.color: parent && parent.activeFocus ? combox.palette.highlight : combox.palette.button;
//                color: combox.palette.base;

//            }
        }
        background: Rectangle {
            width: combox.width
            height: combox.height
            color: parent.hovered ? _checked_or_content_background_color: "white"
            border.width: 1
            border.color: '#DDDDDD'
            radius: 8
        }

        /*下拉图标*/
        indicator: Item {
            id: canvas
            x: combox.width - 20
            y: (combox.height -height)/2
            width: 8
            height: 8
            Image {
                rotation: combox.popup.visible  ? 180 : 0
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                source: "qrc:/images/combox_triangle.png"
            }
        }

        /*下拉框的选项*/
        delegate: ItemDelegate {

            width: combox.width
            height: combox.height
            contentItem: Rectangle
            {

                anchors.fill:parent
                color: parent.hovered ? _checked_or_content_background_color: "white"
                border.color: parent.hovered ? _check_color : '#DDDDDD'
                border.width: 1
                radius: 8
                height:combox.height
                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.text ? model.text : modelData
                    color: parent.parent.hovered ?"white":"gray"
                    font: combox.font
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                }
              //  opacity:parent.pressed ? 0.5 : 1.0

            }
            onPressedChanged: {
                show_message(my_tr(lange_index,"正在切换语言"))
            }
        }

        /*点击后弹出框背景*/
        popup: Popup {
            id:popup
            y: combox.height
            width: combox.width
            height: listview.contentHeight
            padding: 0

            contentItem: ListView {
                header: Rectangle{
                    width: listview.width
                    height: 1
                    color: "#00000000"
                }
                footer:  Rectangle{
                                    width: listview.width
                                    height: 1
                                    color: "#00000000"
                                }

                width: parent.width
                //  anchors.horizontalCenter: parent.horizontalCenter
                spacing: 1
                id: listview
                clip: true
                model: combox.popup.visible ? combox.delegateModel : null
                currentIndex: combox.highlightedIndex


                //  ScrollIndicator.vertical: ScrollIndicator { }
            }

            background: Rectangle {
                color: "#00000000"
            }

        }

        onActivated: {
            let odject = model.get(index)
            if (index  != setting_manage.getIndex())
            {
                setting_manage.setLanguage(odject.key.toString())
                combox_index = -1
                combox_index =setting_manage.getIndex()
                show_message(my_tr(lange_index,"语言切换成功"))
            }else{
            current_Index()
            }
        }
    }
}






