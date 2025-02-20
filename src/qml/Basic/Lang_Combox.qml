import QtQuick 2.14
import QtQuick.Controls 2.14
import "qrc:/src/qml/Css/" as Css

    ComboBox {
        id:combox
        /*显示的文字*/
        font.pixelSize: 18;
        font.family: "微软雅黑"
        width: 280; height: 40;
        opacity: pressed ? 0.5 :1.0
        editable: false
        enabled: true

        contentItem: TextField {
            id:input_text
            leftPadding: !combox.mirrored ? 12 : combox_editable && activeFocus ? 3 : 1;
            rightPadding: combox.mirrored ? 12 : combox_editable && activeFocus ? 3 : 1;
            topPadding: 6 - combox.padding;
            bottomPadding: 6 - combox.padding;
            text: my_tr(lange_index, combox.displayText)
            enabled: false
            anchors.horizontalCenter: combox.horizontalCenter
            anchors.horizontalCenterOffset:  -17
            anchors.left: combox.left
            font: combox.font;
            color: parent.hovered ?"white":"gray"
            selectionColor: combox.palette.highlight;
            selectedTextColor: combox.palette.highlightedText;
            verticalAlignment: Text.AlignVCenter;
            //renderType: Text.NativeRendering;
            background: Rectangle{color: "transparent"}
        }

        background: Rectangle {
            width: combox.width
            height: combox.height
            color: parent.hovered ? _checked_or_content_background_color: "white"
            border.width: 1
            border.color: '#DDDDDD'
            radius: 8
        }

        /*下拉图标*/
        indicator: Item {
            id: canvas
            x: combox.width - 20
            y: (combox.height -height)/2
            width: 8
            height: 8
            Image {
                rotation: combox.popup.visible  ? 180 : 0
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                source: "qrc:/images/combox_triangle.png"
            }
        }

        /*下拉框的选项*/
        delegate: ItemDelegate {

            width: combox.width
            height: combox.height
            contentItem: Rectangle
            {

                anchors.fill:parent
                color: parent.hovered ? _checked_or_content_background_color: "white"
                border.color: parent.hovered ? _check_color : '#DDDDDD'
                border.width: 1
                radius: 8
                height:combox.height
                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    text: my_tr(lange_index, modelData)
                    color: parent.parent.hovered ?"white":"gray"
                    font: combox.font
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                }

            }
        }

        /*点击后弹出框背景*/
        popup: Popup {
            id:popup
            y: combox.height
            width: combox.width
            height: listview.contentHeight
            padding: 0

            contentItem: ListView {
                header: Rectangle{
                    width: listview.width
                    height: 1
                    color: "#00000000"
                }
                footer:  Rectangle{
                                    width: listview.width
                                    height: 1
                                    color: "#00000000"
                                }

                width: parent.width
                //  anchors.horizontalCenter: parent.horizontalCenter
                spacing: 1
                id: listview
                clip: true
                model: combox.popup.visible ? combox.delegateModel : null
                currentIndex: combox.highlightedIndex


                //  ScrollIndicator.vertical: ScrollIndicator { }
            }

            background: Rectangle {
                color: "#00000000"
            }

        }

    }







