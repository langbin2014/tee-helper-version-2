import QtQuick 2.14
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14
TextField{
    id:root
    property real _step_size: 0.0001
    property real _from: 0.0001
    property real _to: 100
    property real _value: Number(text)

    onTextChanged:{
        let str = text;

        let y = str.indexOf(".");//获取小数点的位置
        if(y > 0) {
            let count = str.length - y;//获取小数点后的个数
            if(count > 6)
            {
                str = str.substring(0,y+6)
                root.text  =  str;
            }
        }

    }

    validator: DoubleValidator {
        notation: DoubleValidator.StandardNotation
        decimals:5
        bottom:0.1
        top:  100.0
    }

    width: 120
    height: 36
    color: "white"
    font.pixelSize: 20
    anchors.verticalCenter: parent.verticalCenter
    background: Rectangle{
        border.width: 2
        border.color: parent.focus ? _checked_or_content_background_color :"#00000000"
        radius: 8
        color: "#252930"
    }

    Button{
        id:button1
        anchors.right: parent.right
        anchors.rightMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -6
        width: 14
        height: 7
        autoRepeat: true
        onClicked: {
            let x = root._value + _step_size
            if(x > _to)
                x = _to
            let str = String(x);

            let y = str.indexOf(".");//获取小数点的位置
            if(y > 0) {
                let count = str.length - y;//获取小数点后的个数
                if(count > 6)
                {
                    str = str.substring(0,y+6)
                }
            }
            root.text  =  str;
        }

        background: Item{
            anchors.fill: parent
            Image {
                id:pic1
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                source: "qrc:/images/combox_triangle.png"
                visible: false
            }
            ColorOverlay{
                anchors.fill: parent
                rotation:  180
                source: pic1
                color: button1.hovered  ? _checked_or_content_background_color : "white"
            }
        }
    }

    Button{
        id:button2
        autoRepeat:true
        anchors.right: parent.right
        anchors.rightMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 6
        onClicked: {
            let x = root._value - _step_size
            if(x < root._from)
                x = root._from
            let str = String(x);
            let y = str.indexOf(".");//获取小数点的位置
            if(y > 0) {
                let count = str.length - y;//获取小数点后的个数
                if(count > 6)
                    str = str.substring(0,y+6)
            }
            root.text  =  str;
        }

        width: 14
        height: 7
        background:  Item{
            anchors.fill: parent
            Image {
                id:pic2
                rotation:  0
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                source: "qrc:/images/combox_triangle.png"
                visible: false
            }
            ColorOverlay{
                anchors.fill: parent
                source: pic2
                color: button2.hovered ? _checked_or_content_background_color : "white"
            }
        }
    }
}
