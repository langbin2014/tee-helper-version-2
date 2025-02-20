import QtQuick 2.14
import QtQuick.Controls 1.6
import QtQuick.Controls 2.14
import "qrc:/src/qml/Css/" as Css
import "qrc:/src/qml/Basic/" as Basic
Row {
    property alias label_text: text.text
    property alias label_width: text.width
    property alias label_height: text.height
    property alias label_color: text.color
    property alias date_width: date.width
    property alias date_height: date.height
    property alias input_text:  date_text.text  //create_date.selectedDate
    property alias date_enable: create_date.enabled
    property string key: ""
    property string control_type: "date"
   // property string languageDate: my_tr(lange_index,"中")
    property alias text_horizontalAlignment: text.horizontalAlignment
    property alias text_fontSizeMode: text.fontSizeMode
    property alias _date: create_date.selectedDate

    spacing: 10

    Text {
        id: text
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 20;
        font.family: "微软雅黑"
        text: "text"
        width: 50
        height: 15
        //fontSizeMode: Text.Fit //自动适应列宽
        color: "white"
    }

    Rectangle {
        id: date
        color: _normal_background_color
        width: 106
        height: 36
//        enabled: false
        opacity: mouse_create_date.pressed ?0.5 : 1.0
        radius: 8
        TextInput {
            id:date_text
            readOnly: true
            leftPadding: 10
            text: create_date.selectedDate.toLocaleDateString(Qt.locale(), "yyyy-MM-dd")
            font.pixelSize:20
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            color: _normal_state_font_color
        }


        Basic.DataPicker
        {

            id: create_date
            //selectedDate: new Date(2000, 0, 1)
            anchors.top: date.bottom
            anchors.horizontalCenter: date.horizontalCenter

            onClicked:visible=false
           // enabled: false
            visible:false

            //text: Qt.locale("en_US")
            onSelectedDateChanged: {
                console.log(selectedDate,"22222222")
            }
        }

        Image {
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: 8
            width: 8
            height: 8
            fillMode: Image.PreserveAspectFit
            source: "qrc:/images/combox_triangle.png"
            rotation: create_date.visible ? 180 : 0
        }
        MouseArea{
            id:mouse_create_date
            anchors.fill: parent
            hoverEnabled: true
            onEntered: date.color = _checked_or_content_background_color
            onExited:  date.color = _normal_background_color
            onClicked: {
                create_date.visible = !create_date.visible //&& create_date.enabled
//                        if(create_date.visible&&date_2.visible)
//                            date_2.visible = false
            }

        }
    }
}






