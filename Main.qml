import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Window {
    width: 1000
    height: 700
    visible: true
    title: "Exam Scheduler 2.0 - File Loading"
    color: "#f4f6f8"

    FileDialog {
        id: coursesDialog
        title: "Select courses.txt"
        nameFilters: ["Text files (*.txt)", "All files (*)"]

        onAccepted: {
            appController.setCoursesFilePath(selectedFile)
        }
    }

    FileDialog {
        id: periodsDialog
        title: "Select exam_periods.txt"
        nameFilters: ["Text files (*.txt)", "All files (*)"]

        onAccepted: {
            appController.setExamPeriodsFilePath(selectedFile)
        }
    }

    Rectangle {
        anchors.fill: parent
        anchors.margins: 32
        radius: 12
        color: "white"
        border.color: "#dcdfe3"
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 32
            spacing: 22

            Text {
                text: "Exam Scheduler - Input Screen"
                font.pixelSize: 32
                font.bold: true
                color: "#1f2937"
                Layout.alignment: Qt.AlignHCenter
            }

            Text {
                text: "Select the courses file and the exam periods file, then load the data."
                font.pixelSize: 16
                color: "#4b5563"
                Layout.alignment: Qt.AlignHCenter
            }

            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: "#e5e7eb"
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 14

                Text {
                    text: "Courses File"
                    font.pixelSize: 15
                    font.bold: true
                    color: "#374151"
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    TextField {
                        Layout.fillWidth: true
                        readOnly: true
                        text: appController.coursesFilePath
                        placeholderText: "No courses file selected"
                    }

                    Button {
                        text: "Browse"
                        onClicked: coursesDialog.open()
                    }
                }

                Text {
                    text: "Exam Periods File"
                    font.pixelSize: 15
                    font.bold: true
                    color: "#374151"
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    TextField {
                        Layout.fillWidth: true
                        readOnly: true
                        text: appController.examPeriodsFilePath
                        placeholderText: "No exam periods file selected"
                    }

                    Button {
                        text: "Browse"
                        onClicked: periodsDialog.open()
                    }
                }
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 16

                Button {
                    text: "Replace Data"
                    width: 160
                    onClicked: appController.replaceData()
                }

                Button {
                    text: "Append Data"
                    width: 160
                    onClicked: appController.appendData()
                }
            }

            Rectangle {
                Layout.fillWidth: true
                height: 120
                radius: 8
                color: "#f9fafb"
                border.color: "#e5e7eb"

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 8

                    Text {
                        text: "Current Data Summary"
                        font.pixelSize: 16
                        font.bold: true
                        color: "#111827"
                    }

                    Text {
                        text: "Courses loaded: " + appController.courseCount
                        font.pixelSize: 14
                        color: "#374151"
                    }

                    Text {
                        text: "Exam periods loaded: " + appController.examPeriodCount
                        font.pixelSize: 14
                        color: "#374151"
                    }
                }
            }

            Text {
                Layout.fillWidth: true
                text: appController.statusMessage
                visible: appController.statusMessage.length > 0
                color: "#047857"
                font.pixelSize: 15
                wrapMode: Text.WordWrap
            }

            Text {
                Layout.fillWidth: true
                text: appController.errorMessage
                visible: appController.errorMessage.length > 0
                color: "#b91c1c"
                font.pixelSize: 15
                wrapMode: Text.WordWrap
            }

            Item {
                Layout.fillHeight: true
            }

            Text {
                text: "TES-55 | Main Screen & File Loading"
                color: "#6b7280"
                font.pixelSize: 13
                Layout.alignment: Qt.AlignRight
            }
        }
    }
}