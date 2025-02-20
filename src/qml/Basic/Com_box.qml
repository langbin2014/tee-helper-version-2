import QtQuick 2.14
import QtQuick.Controls 2.14
import "qrc:/src/qml/Css/" as Css
ComboBox {
    id:control
    property color _check_color
    property color _line_color
    /*显示的文字*/
    contentItem: Text {
        leftPadding: 4
        text: control.displayText
        font: control.font
        color: (control.popup.visible | parent.hovered ) ? _check_color : _line_color
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    /*显示的背景*/
    background: Rectangle {
        implicitWidth: control.width
        implicitHeight: control.height
        color: _normal_background_color
        border.color:(control.popup.visible | parent.hovered ) ? _check_color : _line_color
        border.width: 1
        radius: 4
    }

    /*下拉图标*/
    indicator: Item {
        id: canvas
        x: control.width - 20
        y: (control.height -height)/2
        width: 8
        height: 8
        Css.Vote_Arrow{
            _line_color:control.contentItem.color
            anchors.fill:parent
            down:!control.popup.visible
        }
    }

    /*下拉框的选项*/
    delegate: ItemDelegate {
        width: control.width
        height: control.height
        contentItem: Rectangle
        {
            anchors.fill:parent
            color: _normal_background_color
            border.color: parent.hovered ? _check_color : _line_color
            border.width: 1
            radius: 4
            height:control.height
            Text {
                anchors.centerIn: parent
                text: modelData
                color: parent.parent.hovered  ? _check_color : _line_color
                font: control.font
                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    /*点击后弹出框背景*/
    popup: Popup {
        y: control.height
        width: control.width
        implicitHeight: listview.contentHeight
        padding: 0

        contentItem: ListView {
            width: parent.width
            //  anchors.horizontalCenter: parent.horizontalCenter
            spacing: 4
            id: listview
            clip: true
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex

            //  ScrollIndicator.vertical: ScrollIndicator { }
        }

        background: Rectangle {
            color: _checked_or_content_background_color
        }
    }
}




