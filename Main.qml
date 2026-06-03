import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Window {
    id: root

    width: 1200
    height: 760
    minimumWidth: 1050
    minimumHeight: 700
    visible: true
    title: "Exam Scheduler 2.0 - Input"
    color: "#f7f5ef"

    property color primary: "#14533f"
    property color primaryDark: "#0f3f30"
    property color primarySoft: "#edf7f2"
    property color borderSoft: "#e1e5df"
    property color textDark: "#1f2933"
    property color textMuted: "#69737a"
    property color danger: "#b91c1c"
    property color success: "#047857"

    component AppButton: Button {
        id: control

        property bool outline: false

        implicitWidth: 165
        implicitHeight: 46
        font.pixelSize: 15
        font.bold: true

        contentItem: Text {
            text: control.text
            color: control.outline ? root.primary : "white"
            font.pixelSize: 15
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        background: Rectangle {
            radius: 12
            color: control.outline
                   ? (control.down ? "#e3efe9" : control.hovered ? "#f3faf6" : "white")
                   : (control.down ? root.primaryDark : control.hovered ? "#1b664f" : root.primary)
            border.color: root.primary
            border.width: 1
        }
    }

    component FileCard: Rectangle {
        id: card

        property string titleText: ""
        property string descriptionText: ""
        property string iconText: ""
        property string selectedPath: ""
        signal browseClicked()

        Layout.fillWidth: true
        Layout.preferredHeight: 215

        radius: 18
        color: "white"
        border.color: selectedPath.length > 0 ? root.primary : root.borderSoft
        border.width: selectedPath.length > 0 ? 1.5 : 1
        clip: true

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 14

            RowLayout {
                Layout.fillWidth: true
                spacing: 14

                Rectangle {
                    width: 46
                    height: 46
                    radius: 14
                    color: root.primarySoft

                    Text {
                        anchors.centerIn: parent
                        text: card.iconText
                        font.pixelSize: 23
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 4

                    Text {
                        Layout.fillWidth: true
                        text: card.titleText
                        color: root.textDark
                        font.pixelSize: 23
                        font.bold: true
                        horizontalAlignment: Text.AlignRight
                    }

                    Text {
                        Layout.fillWidth: true
                        text: card.descriptionText
                        color: root.textMuted
                        font.pixelSize: 14
                        wrapMode: Text.WordWrap
                        horizontalAlignment: Text.AlignRight
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 78
                radius: 14
                color: selectedPath.length > 0 ? "#f8faf9" : "#f2f7f5"
                border.color: selectedPath.length > 0 ? "#cbded5" : "#d3ddd8"
                border.width: 1

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: card.browseClicked()
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 12

                    Rectangle {
                        width: 42
                        height: 42
                        radius: 12
                        color: selectedPath.length > 0 ? "#dcfce7" : "#e7f3ee"

                        Text {
                            anchors.centerIn: parent
                            text: selectedPath.length > 0 ? "✓" : "↑"
                            color: selectedPath.length > 0 ? root.success : root.primary
                            font.pixelSize: 24
                            font.bold: true
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 3

                        Text {
                            Layout.fillWidth: true
                            text: selectedPath.length > 0 ? "קובץ נבחר בהצלחה" : "בחר קובץ"
                            color: root.textDark
                            font.pixelSize: 16
                            font.bold: true
                            horizontalAlignment: Text.AlignRight
                        }

                        Text {
                            Layout.fillWidth: true
                            LayoutMirroring.enabled: false
                            text: selectedPath.length > 0 ? selectedPath : "לחץ כאן לבחירת קובץ txt"
                            color: root.textMuted
                            font.pixelSize: 12
                            elide: Text.ElideMiddle
                            horizontalAlignment: selectedPath.length > 0 ? Text.AlignLeft : Text.AlignRight
                        }
                    }
                }
            }
        }
    }

    component StatBox: Rectangle {
        id: stat

        property string titleText: ""
        property string valueText: ""
        property string iconText: ""

        Layout.fillWidth: true
        Layout.preferredHeight: 108

        radius: 16
        color: "white"
        border.color: root.borderSoft
        border.width: 1

        RowLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 14

            Rectangle {
                width: 46
                height: 46
                radius: 14
                color: root.primarySoft

                Text {
                    anchors.centerIn: parent
                    text: stat.iconText
                    font.pixelSize: 22
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4

                Text {
                    Layout.fillWidth: true
                    text: stat.valueText
                    color: root.textDark
                    font.pixelSize: 31
                    font.bold: true
                    horizontalAlignment: Text.AlignRight
                }

                Text {
                    Layout.fillWidth: true
                    text: stat.titleText
                    color: root.textMuted
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignRight
                }
            }
        }
    }
    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: inputPage
    }

    Component {
        id: inputPage

        Item {

            FileDialog {
                id: coursesDialog
                title: "בחר קובץ קורסים"
                nameFilters: ["Text files (*.txt)", "All files (*)"]
                onAccepted: appController.setCoursesFilePath(selectedFile)
            }

            FileDialog {
                id: periodsDialog
                title: "בחר קובץ תאריכים"
                nameFilters: ["Text files (*.txt)", "All files (*)"]
                onAccepted: appController.setExamPeriodsFilePath(selectedFile)
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 34
                spacing: 22

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 6

                    Text {
                        Layout.fillWidth: true
                        text: "דף הבית / קלט נתונים"
                        color: "#9aa0a6"
                        font.pixelSize: 13
                        horizontalAlignment: Text.AlignRight
                    }

                    Text {
                        Layout.fillWidth: true
                        text: "קלט נתונים"
                        color: root.textDark
                        font.pixelSize: 36
                        font.bold: true
                        horizontalAlignment: Text.AlignRight
                    }

                    Text {
                        Layout.fillWidth: true
                        text: "טען קבצי קורסים ותאריכים, ולאחר מכן בחר האם להחליף את הנתונים הקיימים או להוסיף אותם למידע הקיים."
                        color: root.textMuted
                        font.pixelSize: 16
                        horizontalAlignment: Text.AlignRight
                        wrapMode: Text.WordWrap
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 24

                    FileCard {
                        titleText: "קובץ תאריכים"
                        descriptionText: "קובץ המגדיר את תקופות הבחינות, מועדים, תאריכים חסומים וחלונות שיבוץ."
                        iconText: "📅"
                        selectedPath: appController.examPeriodsFilePath
                        onBrowseClicked: periodsDialog.open()
                    }

                    FileCard {
                        titleText: "קובץ קורסים"
                        descriptionText: "קובץ המכיל את רשימת הקורסים, תוכניות הלימוד, שנות הלימוד וסוגי הדרישה."
                        iconText: "📚"
                        selectedPath: appController.coursesFilePath
                        onBrowseClicked: coursesDialog.open()
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 178
                    radius: 18
                    color: "white"
                    border.color: root.borderSoft
                    border.width: 1

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 24
                        spacing: 16

                        RowLayout {
                            Layout.fillWidth: true

                            Rectangle {
                                width: 78
                                height: 30
                                radius: 15
                                color: root.primary

                                Text {
                                    anchors.centerIn: parent
                                    text: appController.courseCount + " / " + appController.examPeriodCount
                                    color: "white"
                                    font.pixelSize: 13
                                    font.bold: true
                                }
                            }

                            Text {
                                Layout.fillWidth: true
                                text: "סיכום נתונים שנטענו"
                                color: root.textDark
                                font.pixelSize: 23
                                font.bold: true
                                horizontalAlignment: Text.AlignRight
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 18

                            StatBox {
                                titleText: appController.hasData ? "המערכת מוכנה להמשך" : "טרם נטענו נתונים"
                                valueText: appController.hasData ? "מוכן" : "ממתין"
                                iconText: appController.hasData ? "✅" : "ℹ️"
                            }

                            StatBox {
                                titleText: "תקופות בחינה נטענו"
                                valueText: String(appController.examPeriodCount)
                                iconText: "🗓️"
                            }

                            StatBox {
                                titleText: "קורסים נטענו"
                                valueText: String(appController.courseCount)
                                iconText: "📘"
                            }
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 92
                    radius: 16
                    color: appController.errorMessage.length > 0
                           ? "#fef2f2"
                           : appController.statusMessage.length > 0
                             ? "#ecfdf5"
                             : "white"
                    border.color: appController.errorMessage.length > 0
                                  ? "#fecaca"
                                  : appController.statusMessage.length > 0
                                    ? "#bbf7d0"
                                    : root.borderSoft
                    border.width: 1

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 14

                        AppButton {
                            text: "החלף נתונים"
                            outline: false
                            onClicked: appController.replaceData()
                        }

                        AppButton {
                            text: "הוסף נתונים"
                            outline: true
                            onClicked: appController.appendData()
                        }

                        AppButton {
                            text: "📅 View Calendar"
                            outline: false
                            visible: appController.hasData
                            onClicked: {
                                calendarManager.setData(
                                    appController.examPeriods,
                                    appController.courses
                                )
                                stackView.push("CalendarScreen.qml")
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        Text {
                            Layout.maximumWidth: 620
                            text: appController.errorMessage.length > 0
                                  ? "⚠️ " + appController.errorMessage
                                  : appController.statusMessage.length > 0
                                    ? "✅ " + appController.statusMessage
                                    : "בחר קבצי קלט כדי להתחיל"
                            color: appController.errorMessage.length > 0
                                   ? root.danger
                                   : appController.statusMessage.length > 0
                                     ? root.success
                                     : root.textMuted
                            font.pixelSize: 14
                            font.bold: appController.statusMessage.length > 0 || appController.errorMessage.length > 0
                            horizontalAlignment: Text.AlignRight
                            wrapMode: Text.WordWrap
                        }
                    }
                }

                Text {
                    Layout.fillWidth: true
                    text: "TES-55 | Main Screen & File Loading"
                    color: "#8b949e"
                    font.pixelSize: 13
                    horizontalAlignment: Text.AlignLeft
                }
            }
        }
    }
}