import QtQuick 2.0
import QtQuick.Controls 2.0
CheckBox {
    clip:true
    hoverEnabled: true
    indicator: null
    contentItem: null
    id:root
    background: Rectangle{
        radius: 4
        color: (parent.hovered | parent.checked) ?  _checked_state_font_color : _normal_background_color
        Text {
            //anchors.left: parent.left
            //anchors.leftMargin: 8
            //anchors.verticalCenter: parent.verticalCenter
            font: root.font
            text: root.text
            color: (parent.parent.hovered | parent.parent.checked) ? "black" : "white"
          //  anchors.centerIn: parent


            anchors.fill: parent
            minimumPixelSize:10
            fontSizeMode:Text.HorizontalFit
            verticalAlignment :Text.AlignVCenter
            horizontalAlignment:Text.AlignHCenter
        }
    }

}
