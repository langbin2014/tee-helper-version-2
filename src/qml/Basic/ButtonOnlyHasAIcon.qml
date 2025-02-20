import QtQuick 2.0
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.14
Button {

    property string _source: ""
    property int _icon_margin: 0
    property color _background_color: "#00000000"
    property alias _tip_text: tool_tip_text.text
    property color _hover_color: _checked_state_font_color
    hoverEnabled: true
    indicator: null
    contentItem: null
    id:root
    background: Rectangle{
        color: _background_color
        anchors.fill: parent
        Image {
            id:pic
            anchors.fill: parent
            anchors.margins: _icon_margin
            fillMode: Image.PreserveAspectFit
        source: _source
        visible: false
    }
        ColorOverlay{
            anchors.fill: pic
            source: pic
           color: root.hovered ? _hover_color : "white"
        }
    }


    ToolTip{
        id:tool_tip
        visible: parent.hovered
        contentItem: Text{
            id:tool_tip_text
            text: ""
            font.pixelSize: 12
            font.bold: true
            color: "black"
        }
        delay: 500
        background: Rectangle {
            border.color: "white"
            radius: 4
        }
    }

}
