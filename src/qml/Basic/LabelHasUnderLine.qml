import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Shapes 1.14
Item{
    property alias text: text_item.text
    property alias font: text_item.font
    property int _tag_width: 50
    property string _tag_text: ""
    property color _tag_color: _normal_state_font_color
    id:root
    Text {
        id: tag
        width:_tag_width
        height:parent.height
        text: _tag_text
        anchors.left: parent.left
        anchors.top: parent.top
        color: _tag_color
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignRight
        font.pixelSize: text_item.font.pixelSize * 1.2
        font.bold: text_item.font.bold
    }

    Text{
        id:text_item
        width:parent.width-_tag_width
        height:parent.height
        anchors.top: parent.top
        anchors.left: tag.right
        anchors.leftMargin: 10
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        color: _tag_color
    }
    Shape {
        anchors.fill: parent
        opacity: 0.5
        ShapePath {
            id:path1
            capStyle: ShapePath.RoundCap
            joinStyle: ShapePath.RoundJoin
            strokeWidth: 1
            strokeColor: _tag_color
            startX: 20
            startY: root.height-1
            PathLine{
                x:root.width -20
                y:root.height-1
            }
        }
    }
}


