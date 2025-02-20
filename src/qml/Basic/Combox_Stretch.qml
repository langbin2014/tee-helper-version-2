import QtQuick 2.0
import QtQuick.Layouts 1.14
RowLayout {
    spacing: 0
    property real _factor: 1.2
 //   property int _tag_width: 50
    property string _tag_text: ""
    property color _tag_color: _normal_state_font_color
    property alias _model: combox.model
    property alias _display_text: combox.displayText
    property alias _current_index: combox.currentIndex
    property alias _popup_visible: combox._popup_visible
    Layout.maximumWidth: tag.implicitWidth + 230
    Layout.minimumWidth: tag.implicitWidth/2 + 40
    property alias font: combox.font
    property var min_width: 40
    property alias text_horizontalAlignment: tag.horizontalAlignment
    clip: true
    anchors.leftMargin: 5
    Item{
        Layout.fillWidth: true
        Layout.minimumWidth: tag.implicitWidth
        Layout.fillHeight: true
        clip: true
        Text {
            id: tag
            anchors.right: parent.right
//          width:_tag_width
            text: _tag_text
            color: _tag_color
            anchors.rightMargin: 5
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: combox.font.pixelSize * _factor
            font.bold: combox.font.bold
        }
    }
    Combox_2{
       //Layout.maximumWidth: 128
         Layout.fillWidth: true
       Layout.preferredWidth: 128
       Layout.minimumWidth : min_width
       Layout.fillHeight: true
        id:combox
        _line_color:_tag_color
        _check_color: _checked_state_font_color
    }

}
