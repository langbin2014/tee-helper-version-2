import QtQuick 2.14
import QtQuick.Controls 2.14
import "qrc:/src/qml/Css/" as Css
ComboBox {
    id:control
    property color _check_color
    property color _line_color
    property alias _popup_visible: control.popup.visible
    property var _radius: 4
    property bool _is_set: false
    property color _background_color: "#fff"
    /*显示的文字*/
    opacity:  _is_set ? 1.0 : pressed ? 0.5 :1.0
    contentItem: Text {
        leftPadding: 4
        text: control.displayText
        font: control.font
        color: /*(control.popup.visible | parent.hovered ) ? _check_color :*/ _line_color
        //horizontalAlignment: Text.AlignLeft
      //  horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    /*显示的背景*/
    background: Rectangle {
        width: control.width
        height: control.height
        color: _is_set ? _background_color : parent.hovered ? _checked_or_content_background_color: _normal_background_color
        radius: _radius
    }

    /*下拉图标*/
    indicator: Item {
        id: canvas
        x: control.width - 20
        y: (control.height -height)/2
        width: 8
        height: 8
        Image {
            rotation: control.popup.visible  ? 180 : 0
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            source: "qrc:/images/combox_triangle.png"
        }
    }

    /*下拉框的选项*/
    delegate: ItemDelegate {
        width: control.width
        height: control.height
        contentItem: Rectangle
        {
            anchors.fill:parent
            color: _is_set ? _background_color : parent.hovered ? _checked_or_content_background_color: _normal_background_color
//            border.color: parent.hovered ? _check_color : _line_color
//            border.width: 1
           // radius: 4
            height:control.height
            Text {
                anchors.left: parent.left
             //   anchors.horizontalCenter: parent.horizontalCenter
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                text: modelData
                color:/* parent.parent.hovered  ? _check_color :*/ _line_color
                font: control.font
                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter
            }
          //  opacity:parent.pressed ? 0.5 : 1.0

        }
    }

    /*点击后弹出框背景*/
    popup: Popup {
        y: control.height
        width: control.width
        height: listview.contentHeight > control.height * 10 ? control.height * 10 : listview.contentHeight
        padding: 0

        contentItem: ListView {
            header: Rectangle{
                width: listview.width
                height: 1
                color: "#0b4487"
            }
            footer:  Rectangle{
                                width: listview.width
                                height: 1
                                color: "#0b4487"
                            }

            width: parent.width
            //  anchors.horizontalCenter: parent.horizontalCenter
            spacing: 1
            id: listview
            clip: true
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex

            //  ScrollIndicator.vertical: ScrollIndicator { }
        }

        background: Rectangle {
            color: "#0b4487"
        }
    }
}




