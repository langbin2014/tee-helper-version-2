import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4
Item{
    property real _factor: 1.2
    property int _tag_width: 50
    property string _tag_text: ""
    property color _tag_color: _normal_state_font_color
    property alias _model: combox.model
    property alias _display_text: combox.displayText
    property alias _current_index: combox.currentIndex

    property alias font: combox.font

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
        font.pixelSize: combox.font.pixelSize * _factor
        font.bold: combox.font.bold
    }

    Combox_2{
        id:combox
        width:parent.width-_tag_width - 10
        height:parent.height
        anchors.top: parent.top
        anchors.left: tag.right
        anchors.leftMargin: 10
        _line_color:_tag_color
        _check_color: _checked_state_font_color
    }
}


