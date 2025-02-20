import QtQuick 2.0
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.14
Button{
    contentItem: null
    id:ms
    hoverEnabled: true
    height: _title_height*0.8
    width: height
    property color _hover_color : _checked_or_content_background_color
    background:Item{
        anchors.fill: parent
        Image{
            id:temp_pic1
            source: "qrc:/images/close.png"
            width: sourceSize.width
            height: sourceSize.height
            anchors.centerIn: parent
            opacity: ms.pressed ? 0.5 : 1.0
            visible: true
        }
        ColorOverlay {
            anchors.fill:  temp_pic1
            source: temp_pic1
            color: ms.hovered ? _hover_color : _normal_state_font_color
        }
    }
}
