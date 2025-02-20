//import QtQuick 2.14
//import QtQuick.Controls 1.6
//import QtQuick.Controls 2.14
//Calendar {
//    id:root

//   // visibleYear: 1;
//  //  locale: settings.langage_index_int === 0 ?  Qt.locale() : Qt.locale("en_US")

////    onSelectedDateChanged: {
////        var year = selectedDate.toLocaleDateString(Qt.locale(), "yyyy");
////        var month = selectedDate.toLocaleDateString(Qt.locale(), "MM");
////        var index = 0;
////        for (var i = 1900; i < 2100; i++) {
////            if (i == year) {
////                year_combox.currentIndex = index
////                break;
////            }
////            ++index;
////        }
////        index = 0;
////        for (i = 1; i < 13; i++) {
////            if (i == month) {
////                month_combox.currentIndex = index
////                break;
////            }
////            ++index;
////        }
////    }

////    minimumDate: new Date(1900, 0, 1)
////    maximumDate: new Date(2099, 0, 1)
//  //  selectedDate: new Date(2000, 0, 1)

//    onClicked:visible=false

////    Combox_2 {
////        id:year_combox
////        anchors.left: parent.left
////        width: parent.width / 2
////        height: 30
////        _radius: 0
////        _is_set: true
////        model: {
////            var array = [];
////            for (var i = 1900; i < 2100; ++i) {
////                array.push(i.toString());
////            }
////            model = array
////        }
////        onCurrentTextChanged: {
////            var month = selectedDate.toLocaleDateString(Qt.locale(), "MM");
////            --month;
////            selectedDate = new Date(currentText, month, 1)
////        }
////    }
////    Combox_2 {
////        id: month_combox
////        anchors.right: parent.right
////        width: parent.width / 2
////        height: 30
////        _radius: 0
////        _is_set: true
////        model:["01","02","03","04","05","06","07",
////            "08","09","10","11","12"]
////        onCurrentTextChanged: {
////            var year = selectedDate.toLocaleDateString(Qt.locale(), "yyyy");
////            selectedDate = new Date(year, currentIndex, 1)
////        }
////    }
//}
import QtQuick.Controls 1.6
Calendar {
    minimumDate: new Date(1900, 0, 1)
 //   maximumDate: new Date(2099, 0, 1)
  //  selectedDate: new Date(2000, 0, 1)
    locale: Qt.locale()
    onClicked:visible=false

}
