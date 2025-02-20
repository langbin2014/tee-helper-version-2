import QtQuick 2.0
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.14
CheckBox {
    property string _source: ""
    property int _icon_margin: 0
    property color _background_color: "#00000000"
    property color _icon_color: _checked_state_font_color
    property alias _tip_text: tool_tip_text.text
    property bool _rotate: false
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

            fillMode: Image.PreserveAspectFit

        source: _source
        visible: false

    }
        ColorOverlay{
            anchors.fill: parent
             anchors.margins: _icon_margin
            source: pic
           color: root.hovered | root.checked ? _icon_color : "white"
            rotation: _rotate ? 180 : 0
        }
    }
    ToolTip{
        id:tool_tip
        visible: parent.hovered
        delay: 500
        contentItem: Text{
            id:tool_tip_text
            text: my_tr(lange_index,modelData.hint)
            font.pixelSize: 12
            font.bold: true
            color: "black"
        }

        background: Rectangle {
            border.color: "white"
            radius: 4
            //opacity: 0.3
        }
    }

}
