import QtQuick
import QtQuick.Controls
import QtQuick.Layouts


Item {
    id: outputRoot

    // dialog for choosing where to save the exported schedule
    FileDialog {
        id: saveDialog
        title: "בחר היכן לשמור את המערכת"
        fileMode: FileDialog.SaveFile
        nameFilters: ["Text files (*.txt)"]
        
        onAccepted: {
            let path = saveDialog.selectedFile.toString().replace("file:///", "")
            
            if (appController.outputManager.saveCurrentScheduleToFile(path)) {
                console.log("File saved successfully to: " + path)
            } else {
                console.log("Failed to save!")
            }
        }
    }


    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 34
        spacing: 16

        // export + back buttons
        RowLayout {
            Layout.fillWidth: true
            layoutDirection: Qt.RightToLeft

            Button {
                text: "← חזור לקלט נתונים"
                font.pixelSize: 16
                font.bold: true
                background: Rectangle {
                    color: parent.down ? "#e2e8f0" : (parent.hovered ? "#f1f5f9" : "transparent")
                    radius: 8
                }
                onClicked: {
                    appController.outputManager.clearData()
                    root.mainStackView.pop()
                }
            }

            Item { Layout.fillWidth: true } // pushes the export button to the left

            // export button
            Button {
                text: "📥 ייצא מערכת זו"
                font.pixelSize: 15
                font.bold: true
                Layout.preferredWidth: 150
                Layout.preferredHeight: 40
                
                background: Rectangle {
                    color: parent.down ? "#0f3f30" : (parent.hovered ? "#1b664f" : "#14533f")
                    radius: 8
                }
                
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.bold: true
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                onClicked: {
                    console.log("Exporting schedule " + outputRoot.currentScheduleIndex)
                    saveDialog.open()
                }
            }
        }

        // navigation between different schedules (if multiple were generated from the input)
        Rectangle {
            Layout.fillWidth: true
            height: 60
            radius: 12
            color: "white"
            border.color: "#e1e5df"
            border.width: 1
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                layoutDirection: Qt.RightToLeft

                Button {
                    text: "< הקודם"
                    font.pixelSize: 15
                    font.bold: true
                    Layout.preferredWidth: 100
                    Layout.preferredHeight: 40
                    enabled: appController.outputManager.currentScheduleIndex > 1
                    
                    background: Rectangle {
                        color: parent.enabled ? (parent.down ? "#e2e8f0" : (parent.hovered ? "#f8fafc" : "white")) : "#f1f5f9"
                        border.color: parent.enabled ? "#cbd5e1" : "#e2e8f0"
                        radius: 8
                    }
                    onClicked: {
                        appController.outputManager.previousSchedule();
                    }
                }

                Text {
                    Layout.fillWidth: true
                    text: "מערכת " + appController.outputManager.currentScheduleIndex + " מתוך " + appController.outputManager.totalSchedulesCount
                    font.pixelSize: 18
                    font.bold: true
                    color: "#1f2933"
                    horizontalAlignment: Text.AlignHCenter
                }

                Button {
                    text: "הבא >"
                    font.pixelSize: 15
                    font.bold: true
                    Layout.preferredWidth: 100
                    Layout.preferredHeight: 40
                    enabled: appController.outputManager.currentScheduleIndex < appController.outputManager.totalSchedulesCount
                    
                    background: Rectangle {
                        color: parent.enabled ? (parent.down ? "#e2e8f0" : (parent.hovered ? "#f8fafc" : "white")) : "#f1f5f9"
                        border.color: parent.enabled ? "#cbd5e1" : "#e2e8f0"
                        radius: 8
                    }
                    onClicked: {
                        appController.outputManager.nextSchedule();
                    }
                }
            }
        }

        // choice of semester/moed for filtering the calendar 
        RowLayout {
            Layout.fillWidth: true
            layoutDirection: Qt.RightToLeft
            spacing: 12
            Layout.topMargin: 8
            Layout.bottomMargin: 4

            Item { Layout.fillWidth: true } 

            Text {
                text: "סמסטר:"
                font.pixelSize: 16
                font.bold: true
                color: "#1f2933"
            }

            ComboBox {
                id: semesterSelector
                Layout.preferredWidth: 120
                Layout.preferredHeight: 40
                font.pixelSize: 15
                font.bold: true
                
                // dynamically linked to the list in C++
                model: appController.outputManager.availableSemesters
                
                background: Rectangle {
                    color: "#f8fafc"
                    border.color: "#cbd5e1"
                    border.width: 1
                    radius: 8
                }
                
                onActivated: {
                    appController.generateForPeriod(currentText, moedSelector.currentText);
                }
            }

            Item { width: 20 } 

            Text {
                text: "מועד:"
                font.pixelSize: 16
                font.bold: true
                color: "#1f2933"
            }

            ComboBox {
                id: moedSelector
                Layout.preferredWidth: 120
                Layout.preferredHeight: 40
                font.pixelSize: 15
                font.bold: true
                
                // dynamically linked to the list in C++
                model: appController.outputManager.availableMoeds
                
                background: Rectangle {
                    color: "#f8fafc"
                    border.color: "#cbd5e1"
                    border.width: 1
                    radius: 8
                }
                
                onActivated: {
                    appController.generateForPeriod(semesterSelector.currentText, currentText);
                }
            }

            Item { Layout.fillWidth: true }
        }

        // Calendar grid
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 16
            color: "white"
            border.color: "#e1e5df"
            border.width: 1
            clip: true

            Item {
                id: weekDaysHeader
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 20
                height: 30

                Row {
                    anchors.fill: parent
                    layoutDirection: Qt.RightToLeft
                    Repeater {
                        model: ["ראשון", "שני", "שלישי", "רביעי", "חמישי", "שישי", "שבת"]
                        Text {
                            width: weekDaysHeader.width / 7
                            text: modelData
                            font.pixelSize: 14
                            font.bold: true
                            color: "#69737a"
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }
            }

            // demo model
            ListModel {
                id: mockCalendarModel
                ListElement { dayText: ""; isExcluded: false; hasExam: false; examName: ""; courseId: ""; req: ""; program: "" }
                ListElement { dayText: ""; isExcluded: false; hasExam: false; examName: ""; courseId: ""; req: ""; program: "" }
                ListElement { dayText: ""; isExcluded: false; hasExam: false; examName: ""; courseId: ""; req: ""; program: "" }
                ListElement { dayText: "1"; isExcluded: false; hasExam: true; examName: "פיזיקה 1"; courseId: "83101"; req: "חובה"; program: "הנדסת מחשבים" }
                ListElement { dayText: "2"; isExcluded: false; hasExam: false; examName: ""; courseId: ""; req: ""; program: "" }
                ListElement { dayText: "3"; isExcluded: true; hasExam: false; examName: ""; courseId: ""; req: ""; program: "" }
                ListElement { dayText: "4"; isExcluded: true; hasExam: false; examName: ""; courseId: ""; req: ""; program: "" }
                
                ListElement { dayText: "5"; isExcluded: false; hasExam: true; examName: "פרויקט תוכנה"; courseId: "83108"; req: "בחירה"; program: "הנדסת תוכנה" }
                ListElement { dayText: "6"; isExcluded: false; hasExam: false; examName: ""; courseId: ""; req: ""; program: "" }
                ListElement { dayText: "7"; isExcluded: false; hasExam: false; examName: ""; courseId: ""; req: ""; program: "" }
                ListElement { dayText: "8"; isExcluded: false; hasExam: false; examName: ""; courseId: ""; req: ""; program: "" }
                ListElement { dayText: "9"; isExcluded: false; hasExam: true; examName: "אינפי 1"; courseId: "83102"; req: "חובה"; program: "הנדסת חשמל" }
                ListElement { dayText: "10"; isExcluded: true; hasExam: false; examName: ""; courseId: ""; req: ""; program: "" }
                ListElement { dayText: "11"; isExcluded: true; hasExam: false; examName: ""; courseId: ""; req: ""; program: "" }

                ListElement { dayText: "12"; isExcluded: false; hasExam: false; examName: ""; courseId: ""; req: ""; program: "" }
                ListElement { dayText: "13"; isExcluded: false; hasExam: true; examName: "מבני נתונים"; courseId: "83107"; req: "חובה"; program: "הנדסת נתונים" }
                ListElement { dayText: "14"; isExcluded: false; hasExam: false; examName: ""; courseId: ""; req: ""; program: "" }
                ListElement { dayText: "15"; isExcluded: false; hasExam: false; examName: ""; courseId: ""; req: ""; program: "" }
                ListElement { dayText: "16"; isExcluded: false; hasExam: false; examName: ""; courseId: ""; req: ""; program: "" }
                ListElement { dayText: "17"; isExcluded: true; hasExam: false; examName: ""; courseId: ""; req: ""; program: "" }
                ListElement { dayText: "18"; isExcluded: true; hasExam: false; examName: ""; courseId: ""; req: ""; program: "" }
            }

            GridView {
                id: calendarGrid
                anchors.top: weekDaysHeader.bottom
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 20
                
                cellWidth: width / 7
                cellHeight: 120
                
                model: appController.outputManager.currentCalendarData
                layoutDirection: Qt.RightToLeft
                interactive: true
                clip: true

                ScrollBar.vertical: ScrollBar {
                    active: true
                    policy: ScrollBar.AsNeeded
                }

                delegate: Rectangle {
                    width: calendarGrid.cellWidth - 10
                    height: calendarGrid.cellHeight - 10
                    radius: 8
                    
                    color: model.dayText === "" ? "transparent" : (model.isExcluded ? "#fef2f2" : (model.hasExam ? "#f8fafc" : "#f1f5f9"))
                    border.color: model.dayText === "" ? "transparent" : (model.isExcluded ? "#fecaca" : (model.hasExam ? (model.req === "חובה" ? "#fca5a5" : "#86efac") : "#e2e8f0"))
                    border.width: 1

                    Text {
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.margins: 8
                        text: modelData.dayText || ""
                        font.pixelSize: 14
                        font.bold: true
                        color: modelData.isExcluded ? "#dc2626" : "#64748b"
                        visible: modelData.dayText !== ""
                    }

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        anchors.topMargin: 24
                        spacing: 2
                        visible: modelData.hasExam === true

                        Text {
                            Layout.fillWidth: true
                            text: modelData.examName || ""
                            font.pixelSize: 13
                            font.bold: true
                            color: "#1f2933"
                            elide: Text.ElideRight
                            horizontalAlignment: Text.AlignRight
                        }

                        Text {
                            Layout.fillWidth: true
                            text: (modelData.courseId || "") + " | " + (modelData.req || "")
                            font.pixelSize: 11
                            color: model.req === "חובה" ? "#b91c1c" : "#047857"
                            horizontalAlignment: Text.AlignRight
                        }

                        Text {
                            Layout.fillWidth: true
                            text: "👤 " + (model.program || "")
                            font.pixelSize: 10
                            color: "#14533f" 
                            elide: Text.ElideRight
                            horizontalAlignment: Text.AlignRight
                        }
                    }
                }
            }
        }
    }
}