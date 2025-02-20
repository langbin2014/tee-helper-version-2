import QtQuick 2.0
import QtQuick.Controls 2.0
Row {
    property string _tag_text: ""
    property string _content_text:""
    property int _tag_widht: 80
    property int _content_width : 200
    spacing: 10
    height: 40
    Rectangle{
        color: "#00000000"
        height: parent.height
        width: _tag_widht
        Text {
            id:tag
            font.pixelSize: 20;
            text: _tag_text
            color: "white"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
        }
    }

    Item{
//        color:"#252930" //"#373E49";//  "#252930"
//        radius: 8
        Rectangle{
            width: parent.width
            height: 2
            anchors.bottom: parent.bottom
            color: "#373E49"
        }

        height: parent.height
        width: _content_width
        Text{
            id: content_text
            text: _content_text
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 6
            font.pixelSize: 20;
            color: "white"
        }
    }
}
