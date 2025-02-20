import QtQuick 2.0
import QtQuick.Layouts 1.14
Row {
    property real _factor: 1.2
    property string _tag_text: ""
    property color _tag_color: _normal_state_font_color
    property alias _model: combox.model
    property alias _current_index: combox.currentIndex
    property alias _popup_visible: combox._popup_visible
  //  property alias _font: combox.font
  //  property alias text_horizontalAlignment: tag.horizontalAlignment
    property int _tag_width:50
    property int _combox_width:120
    property int _combox_height:40
    anchors.leftMargin: 5

        Text {
            font.pixelSize: 20
            font.family: "微软雅黑"
            width: _tag_width
            id: tag
            text: _tag_text
            color: _tag_color
            anchors.verticalCenter: parent.verticalCenter
            font.bold: combox.font.bold
        }

    Combox_2{
        font.pixelSize: 18;
        font.family: "微软雅黑"
        width: _combox_width
        height: _combox_height
        id:combox
        _line_color:_tag_color
        _check_color: _checked_state_font_color
    }

}
