import QtQuick 2.0
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0
Row {
    property alias label_text: label.text
    property alias label_pixelSize: label.font.pixelSize
    property alias label_horizontalAlignment: label.horizontalAlignment


    property alias input_text: input.text
    property alias input_width: input.width
    property alias input_height: input.height
    property alias input_placeholderText: input.placeholderText
    //property alias input_text: input.text
    property alias _input_readonly:input.readOnly
    property color checkedColor: "#252930"
    spacing: 10

    Text {
        id: label
        text: my_tr(lange_index,"text")
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignRight
        font.pixelSize: 20;
        font.family: "微软雅黑"
        width: 70
        height: 36
        color: "white"
    }
    TextField {
        id: input
//        anchors.left: label.right
//        anchors.leftMargin: 10
        font.family: "微软雅黑"
        font.pixelSize: 18
        antialiasing: true
        color: "#fff"
        selectByMouse: true
//        selectionColor: combox.palette.highlight;
//        selectedTextColor: combox.palette.highlightedText;

        background: Rectangle {
            anchors.left: input.left
            implicitWidth: input.width
            implicitHeight: input.height
            radius: 8
            color: parent.hovered ? "#1984fc": "#252930"
        }


     }
}
