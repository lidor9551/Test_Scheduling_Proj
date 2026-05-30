import QtQuick
import QtQuick.Window
import QtQuick.Controls 2.15

Window {
    width: 800
    height: 600
    visible: true
    title: "Exam Scheduler 2.0"
    color: "#2c3e50" 

    Column {
        anchors.centerIn: parent
        spacing: 20

        Text {
            text: "המערכת עובדת!"
            color: "white"
            font.pixelSize: 36
            font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            text: "סביבת ה-QML קומפלה ומחוברת בהצלחה ל-C++"
            color: "#bdc3c7"
            font.pixelSize: 18
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Rectangle {
            width: 200
            height: 50
            color: mouseArea.pressed ? "#2980b9" : "#3498db"
            radius: 8
            anchors.horizontalCenter: parent.horizontalCenter

            Behavior on color { ColorAnimation { duration: 150 } }

            Text {
                anchors.centerIn: parent
                text: "בדיקת תגובתיות"
                color: "white"
                font.pixelSize: 16
                font.bold: true
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                onClicked: console.log("Click registered! UI is fully responsive.")
            }

            Button {
                text: "בדוק חיבור ל-C++"
                onClicked: {
                    console.log("Button clicked, calling C++...")
                    scheduler.startScheduling(-1)
                }
            }
        }
    }
}