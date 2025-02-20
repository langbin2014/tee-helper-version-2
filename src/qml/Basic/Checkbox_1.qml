import QtQuick 2.0
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.14

CheckBox {
    id: root

    property color checkedColor: "#1984fc"//"#252930"//
    property alias check_text: text.text

    //text: my_tr(lange_index,"CheckBox")
    checked:true

    indicator: Rectangle {
        id:check
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        width: 25; height: width
        antialiasing: true //抗锯齿
        radius: 5
        border.width: 2
        border.color: root.checkedColor

        /*Rectangle {
            anchors.centerIn: parent
            width: parent.width*0.7; height: width
            antialiasing: true
            radius: parent.radius * 0.7
            color: root.checkedColor
            visible: root.checked
        }*/
        Image {
            id: pic
            anchors.centerIn: parent
            width: parent.width*0.8;
            height: width
            antialiasing: true
            visible: root.checked//false
            source: "qrc:/images/checked_icon.png"

        }
        /*ColorOverlay{
            anchors.fill: parent
            source: pic
           color: checkedColor
           visible: root.checked
        }*/
    }
    contentItem: Text {
        id:text
        anchors.left: check.right
        anchors.right: parent.right
        anchors.leftMargin: 5
        //anchors.verticalCenter: parent.verticalCenter
        anchors.rightMargin: 20
        font.pixelSize: 18
        color: "white"
    }
}
