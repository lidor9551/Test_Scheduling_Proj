import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: editorScreen
    background: Rectangle { color: "#FAF8F3" }

    header: Rectangle {
        width: parent.width
        height: 64
        color: "#1B4332"

        RowLayout {
            anchors.fill: parent
            anchors.margins: 16

            Button {
                text: "← Back"
                flat: true
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.pixelSize: 15
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle { color: "transparent" }
                onClicked: stackView.pop()
            }

            Text {
                text: "Edit Exam Period — " + calendarManager.currentSemester
                color: "white"
                font.pixelSize: 18
                font.bold: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    Component.onCompleted: {
        startDateField.text = calendarManager.currentStartDate
        endDateField.text   = calendarManager.currentEndDate
    }

    Connections {
        target: calendarManager
        function onSemesterChanged() {
            startDateField.text = calendarManager.currentStartDate
            endDateField.text   = calendarManager.currentEndDate
        }
    }

    function buildPaddedModel(rawDays) {
        if (!rawDays || rawDays.length === 0) return []
        var firstDate = rawDays[0].date
        var dow = firstDate.getDay()
        var padded = []
        for (var i = 0; i < dow; i++)
            padded.push({ date: null, status: -1, label: "" })
        for (var j = 0; j < rawDays.length; j++)
            padded.push(rawDays[j])
        return padded
    }

    function monthLabel(rawDays) {
        if (!rawDays || rawDays.length === 0) return ""
        return Qt.formatDate(rawDays[0].date, "MMMM yyyy")
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 20

        // Date range inputs + buttons
        Rectangle {
            Layout.fillWidth: true
            height: 120
            radius: 12
            color: "white"
            border.color: "#E9ECEF"
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 20

                ColumnLayout {
                    spacing: 6
                    Text { text: "Start Date"; font.pixelSize: 13; color: "#6C757D" }
                    TextField {
                        id: startDateField
                        placeholderText: "YYYY-MM-DD"
                        font.pixelSize: 15
                        background: Rectangle {
                            radius: 8
                            color: "#F8F9FA"
                            border.color: "#1B4332"
                            border.width: 1
                        }
                    }
                }

                ColumnLayout {
                    spacing: 6
                    Text { text: "End Date"; font.pixelSize: 13; color: "#6C757D" }
                    TextField {
                        id: endDateField
                        placeholderText: "YYYY-MM-DD"
                        font.pixelSize: 15
                        background: Rectangle {
                            radius: 8
                            color: "#F8F9FA"
                            border.color: "#1B4332"
                            border.width: 1
                        }
                    }
                }

                // Apply Shift button
                Button {
                    text: "Apply Shift"
                    Layout.alignment: Qt.AlignBottom
                    background: Rectangle {
                        radius: 8
                        color: parent.hovered ? "#52B788" : "#1B4332"
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        font.pixelSize: 14
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: {
                        var start = new Date(startDateField.text)
                        var end   = new Date(endDateField.text)
                        if (isNaN(start.getTime()) || isNaN(end.getTime()) || start > end) {
                            shiftErrorMsg.visible = true
                            shiftErrorTimer.restart()
                            return
                        }
                        calendarManager.shiftPeriod(
                            calendarManager.currentSemester,
                            startDateField.text,
                            endDateField.text
                        )
                    }
                }

                // Save button
                Button {
                    text: "💾 Save"
                    Layout.alignment: Qt.AlignBottom
                    background: Rectangle {
                        radius: 8
                        color: parent.hovered ? "#1a6b45" : "#157347"
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        font.pixelSize: 14
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: {
                        calendarManager.saveChanges()
                        saveConfirm.visible = true
                        saveTimer.restart()
                    }
                }

                // Save confirmation message
                Text {
                    id: saveConfirm
                    text: "✅ Saved"
                    color: "#157347"
                    font.pixelSize: 13
                    font.bold: true
                    visible: false
                }

                Timer {
                    id: saveTimer
                    interval: 2000
                    onTriggered: saveConfirm.visible = false
                }

                // Error message for invalid date range
                Text {
                    id: shiftErrorMsg
                    text: "⚠ Error: Start date is after end date"
                    color: "#C0392B"
                    font.pixelSize: 13
                    font.bold: true
                    visible: false
                }

                Timer {
                    id: shiftErrorTimer
                    interval: 3000
                    onTriggered: shiftErrorMsg.visible = false
                }
            }
        }

        // Calendar grid
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 16
            color: "white"
            border.color: "#E9ECEF"
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 8

                Text {
                    text: "Click a day to exclude / restore it"
                    font.pixelSize: 13
                    color: "#6C757D"
                    font.italic: true
                }

                Text {
                    id: editorMonthTitle
                    text: editorScreen.monthLabel(calendarManager.days)
                    font.pixelSize: 15
                    font.bold: true
                    color: "#1B4332"
                    Layout.alignment: Qt.AlignHCenter
                }

                Row {
                    Layout.fillWidth: true
                    Repeater {
                        model: ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"]
                        delegate: Text {
                            width: Math.floor(editorGrid.width / 7)
                            text: modelData
                            font.pixelSize: 12
                            font.bold: true
                            color: index === 6 ? "#C0392B" : "#6C757D"
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }

                GridView {
                    id: editorGrid
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    cellWidth:  Math.floor(width / 7)
                    cellHeight: 52

                    property var paddedDays: editorScreen.buildPaddedModel(calendarManager.days)
                    model: paddedDays

                    Connections {
                        target: calendarManager
                        function onDaysChanged() {
                            editorGrid.paddedDays = editorScreen.buildPaddedModel(calendarManager.days)
                            editorMonthTitle.text = editorScreen.monthLabel(calendarManager.days)
                        }
                    }

                    delegate: Rectangle {
                        width:  editorGrid.cellWidth  - 4
                        height: editorGrid.cellHeight - 4
                        radius: 8
                        visible: modelData.status !== -1
                        color: {
                            if (modelData.status === 2) return "#FDECEA"
                            if (modelData.status === 1) return "#EAFAF1"
                            return "#F8F9FA"
                        }
                        border.color: {
                            if (modelData.status === 2) return "#C0392B"
                            if (modelData.status === 1) return "#52B788"
                            return "#E9ECEF"
                        }
                        border.width: 1

                        Text {
                            anchors.centerIn: parent
                            text: modelData.date ? Qt.formatDate(modelData.date, "d") : ""
                            font.pixelSize: 14
                            font.bold: true
                            color: modelData.status === 2 ? "#C0392B" : "#1B4332"
                        }

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            enabled: modelData.status !== -1 && modelData.date.getDay() !== 6
                            onClicked: {
                                let dateStr = Qt.formatDate(modelData.date, "yyyy-MM-dd")
                                calendarManager.toggleDay(dateStr)
                            }
                        }
                    }
                }
            }
        }
    }
}
