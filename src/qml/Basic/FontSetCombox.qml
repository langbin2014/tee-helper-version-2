import QtQuick 2.14
import QtQuick.Controls 2.14

ComboBox {
    id:control
    /*显示的文字*/
   // boundsBehavior: Flickable.DragAndOvershootBounds
    Keys.onUpPressed: scrollBar.decrease()
    Keys.onDownPressed: scrollBar.increase()
    property var init_font: "微软雅黑"
    function setInitFont(font){
        init_font = font
        control.displayText = init_font
    }

    contentItem: Text {
        anchors.left: combo_image.right
        anchors.leftMargin: 10
        text: control.displayText
        font.family: init_font
        font.pixelSize: 16
        elide: Text.ElideRight
        color: "#fff"
        verticalAlignment: Text.AlignVCenter
    }
//    ScrollBar.vertical: ScrollBar {
//                id: scrollBar;
//                policy: ScrollBar.AlwaysOn
//        orientation: Qt.Vertical
//            implicitWidth:20
//            }
    /*显示的背景*/
    background: Rectangle {
        implicitWidth: control.width
        implicitHeight: control.height
        color: control.hovered ? "#1984fc": "#252930"
        border.color:(control.popup.visible | parent.hovered ) ? "#fff" : "#232323"
        border.width: 1
        radius: 4
    }
    /*下拉框的选项*/
    delegate: ItemDelegate {
        width: control.width
        height: control.height

        contentItem: Rectangle
        {
            anchors.fill:parent
            color: parent.hovered ? "#fff" : "#232323"
            border.color:  "#fff"
            border.width: 1
//            radius: 4
            height:control.height
            Text {
                anchors.centerIn: parent
                text: modelData
                color: parent.parent.hovered  ? "#232323" :  "#fff"
                font.family: "微软雅黑"
                font.pixelSize: 18
                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

}




