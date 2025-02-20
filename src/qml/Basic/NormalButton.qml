import QtQuick 2.0
import QtQuick.Controls 2.0
Button {
    hoverEnabled: true
    indicator: null
    contentItem: null
    property color _hovered_color: _checked_state_font_color
    property color _hovered_text_color : "black"
    property var button_radius : 4
    property color bg_color :_normal_background_color
    property color tx_color :_normal_state_font_color
    id:root
    font.pixelSize: 18
    font.bold: true

    background: Rectangle{
        radius: button_radius
        //border.width: 1
       // border.color: _normal_state_font_color
        color: parent.hovered ? _hovered_color : bg_color
        Text {
           // anchors.centerIn: parent
            font: root.font
            text: root.text
            color: parent.parent.hovered ? _hovered_text_color  : tx_color

            anchors.fill: parent
            minimumPixelSize:10
            fontSizeMode:Text.HorizontalFit
            verticalAlignment :Text.AlignVCenter
            horizontalAlignment:Text.AlignHCenter
        }
    }

}
