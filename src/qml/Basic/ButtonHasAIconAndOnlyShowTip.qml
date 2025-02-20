import QtQuick 2.0
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.14
Button {

    property string _source: ""
    property int _icon_margin: 0
    property alias _tip_text: tool_tip_text.text
    hoverEnabled: true
    indicator: null
    contentItem: null
    id:root
    background:  Image {
        id:pic
        anchors.fill: parent
        anchors.margins: _icon_margin
        fillMode: Image.PreserveAspectFit
        source: _source
        visible: true
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
