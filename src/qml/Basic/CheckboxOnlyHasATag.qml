import QtQuick 2.0
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.13
CheckBox {
    hoverEnabled: true
//   indicator.x:root.width-indicator.width-10
//   indicator.width:indicator.height
//   indicator.height: root.height*2/3
   indicator: null
    contentItem: null
    property alias _tag_text:root.text
    property color _text_color: "black"
    property color _background_color:  "#aae0e0e0"
    id:root
    background: Rectangle{
        radius: 4
        color:_background_color
        border.color: root.hovered ? _checked_or_content_background_color : "#00000000"
        border.width: 3
        Text {
            anchors.left: parent.left
            anchors.leftMargin: 8
            height: parent.height
            width: parent.width-parent.height-10
            font: root.font
            text: root.text
            color: _text_color

            minimumPixelSize:10
            fontSizeMode: Text.HorizontalFit
            verticalAlignment :Text.AlignVCenter
            horizontalAlignment:Text.AlignLeft
        }
        Rectangle{
            radius: 1
            border.width: 2
            border.color: root.hovered ? _checked_or_content_background_color :  "#aae0e0e0"//"black"
            anchors.right: parent.right
            anchors.rightMargin: 5
            height: parent.height-10
            width: height
            anchors.verticalCenter: parent.verticalCenter

            Image {
                id:pic
                visible: false
                source: "qrc:/images/checked.svg"
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit

            }
            ColorOverlay{
                visible: root.checked
                anchors.fill: pic
                source: pic
               color: _checked_or_content_background_color
            }
        }

    }

}
