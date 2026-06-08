import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: outputRoot
    width: parent.width
    height: parent.height

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 34
        spacing: 20

        // כפתור חזרה למסך הקלט
        Button {
            text: "← חזור לקלט נתונים"
            font.pixelSize: 16
            font.bold: true
            background: Rectangle {
                color: parent.down ? "#e2e8f0" : (parent.hovered ? "#f1f5f9" : "transparent")
                radius: 8
            }
            onClicked: {
                // חוזרים למסך הקודם בסטאק
                StackView.view.pop()
            }
        }

        // סרגל הניווט העליון של המערכות (ייבנה בשלב הבא)
        Rectangle {
            Layout.fillWidth: true
            height: 60
            radius: 12
            color: "white"
            border.color: "#e1e5df"
            
            Text {
                anchors.centerIn: parent
                text: "סרגל ניווט המערכות ייבנה כאן"
                color: "#69737a"
            }
        }

        // אזור ה-Grid של המערכת (ייבנה בהמשך)
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 16
            color: "white"
            border.color: "#e1e5df"
            
            Text {
                anchors.centerIn: parent
                text: "תצוגת הלוח (Grid) תיבנה כאן"
                font.pixelSize: 24
                color: "#69737a"
            }
        }
    }
}