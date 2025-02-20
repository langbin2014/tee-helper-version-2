import QtQuick 2.0
import QtQuick.Controls 2.0
Row {
    id: root
    property alias is_text: text.visible

    property alias label_text: text.text
    property alias label_width:  text.width
    property alias label_height:  text.height
    property alias label_color:  text.color
    property alias text_font_size: text.font.pixelSize
    property alias text_horizontalAlignment: text.horizontalAlignment
    property alias text_fontSizeMode: text.fontSizeMode

    property alias is_text_last: text_last.visible

    property alias label_last_text: text_last.text
    property alias label_last_width: text_last.width
    property alias label_last_height: text_last.height
    property alias label_last_color: text_last.color
    property alias text_last_font_size: text_last.font.pixelSize
    property alias text_last_horizontalAlignment: text_last.horizontalAlignment
    property alias text_last_fontSizeMode: text_last.fontSizeMode

    property alias input_width: input_text.width
    property alias input_height: input_text.height
    property alias input_text: input_text.text
    property alias input_enable: input_text.enabled
    property alias input_readOnly: input_text.readOnly
    property alias input_font_size: input_text.font.pixelSize
    property alias input_echoMode: input_text.echoMode
    property alias _spacing: root.spacing
    property alias _validator:input_text.validator

    property string key: ""
    property string control_type: "input_text"
    signal sigTextChanged(var text)
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
        //text_fontSizeMode: Text.Fit //自动适应列宽
    }

    TextField {
        id: input_text
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 20;
        font.family: "微软雅黑"
        color: "white"
        //cursorVisible: true;
        selectByMouse: true //是否可以选择文本
        selectionColor: "#999999"//选中背景颜色
        text: ""
        readOnly: false
        echoMode: TextInput.Normal
        width: 280; height: 40;

        background: Rectangle {
            border.width: 0; border.color: "red"
            radius: 8; color: "#252930";
//            opacity: 0.05 //透明度
            implicitHeight: 40; implicitWidth: 280
        }
        onTextChanged: {
            sigTextChanged(input_text)
        }
    }


    Text {
        id: text_last
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 20;
        font.family: "微软雅黑"
        text: "text"
        width: 50
        height: 15
        color: "white"
        visible: false
        //text_fontSizeMode: Text.Fit //自动适应列宽
    }

}
