import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Page {
    id: calendarScreen
    FileDialog {

        id: exportDialog

        title: "Export schedule"

        fileMode: FileDialog.SaveFile

        nameFilters: ["Text files (*.txt)", "All files (*)"]

        defaultSuffix: "txt"

        onAccepted: {

            const ok = calendarManager.exportCurrentSchedule(selectedFile)

            if (ok) {

                saveConfirm.text = "✅ Exported"

            } else {

                saveConfirm.text = "❌ Export failed"

            }

            saveConfirm.visible = true

            saveTimer.restart()

        }

    }

    background: Rectangle { color: "#FAF8F3" }
    
    // ── Header ──────────────────────────────────────────────────────────────
    header: Rectangle {
        width: parent.width
        height: 64
        color: "#1B4332"

        RowLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 12

            Button {
                text: "←"
                flat: true
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.pixelSize: 20
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle { color: "transparent" }
                onClicked: stackView.pop()
            }

            Text {
                text: calendarManager.currentSemester
                color: "white"
                font.pixelSize: 20
                font.bold: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }

            // Save changes in the current calendar state
            Button {
                text: "💾 Save"
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle {
                    radius: 8
                    color: parent.hovered ? "#1a6b45" : "#157347"
                }
                onClicked: {
                    calendarManager.saveChanges()
                    saveConfirm.text = "✅ Saved"
                    saveConfirm.visible = true
                    saveTimer.restart()
                }
            }

            // Export the current displayed schedule to a text file
            Button {
                text: "💾 Save as File"
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle {
                    radius: 8
                    color: parent.hovered ? "#1a6b45" : "#157347"
                }
                onClicked: {
                    exportDialog.open()
                }
            }

            Text {
                id: saveConfirm
                text: "✅ Saved"
                color: "white"
                font.pixelSize: 13
                font.bold: true
                visible: false
            }

            Timer {
                id: saveTimer
                interval: 2000
                onTriggered: saveConfirm.visible = false
            }

            Button {
                text: "✏️ Edit Period"
                font.pixelSize: 14
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle {
                    radius: 8
                    color: parent.hovered ? "#52B788" : "transparent"
                    border.color: "white"
                    border.width: 1
                }
                onClicked: stackView.push("PeriodEditorScreen.qml")
            }
        }
    }

    // ── Helper functions ─────────────────────────────────────────────────────
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

    // ── Body: sidebar + calendar ─────────────────────────────────────────────
    RowLayout {
        anchors.fill: parent
        anchors.margins: 0
        spacing: 0

        // ── Left sidebar ─────────────────────────────────────────────────────
        Rectangle {
            width: 200
            Layout.fillHeight: true
            color: "#1B4332"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 4

                Text {
                    text: "Periods"
                    color: "#A8D5B5"
                    font.pixelSize: 12
                    font.bold: true
                    topPadding: 8
                    bottomPadding: 4
                }

                // Tree built from getPeriodTree()
                ListView {
                    id: treeView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    spacing: 2

                    // Group periods by year then semester
                    property var tree: buildTree(calendarManager.getPeriodTree())
                    model: tree

                    // Expanded state: track which years and semesters are open
                    property var expandedYears: ({})
                    property var expandedSemesters: ({})

                    function buildTree(flat) {
                        // Returns array of year-nodes, each with semester-nodes,
                        // each with moed-leaves
                        var years = {}
                        var yearOrder = []
                        for (var i = 0; i < flat.length; i++) {
                            var e = flat[i]
                            var y = String(e.year)
                            var s = e.semester
                            if (!years[y]) {
                                years[y] = {}
                                yearOrder.push(y)
                            }
                            if (!years[y][s]) years[y][s] = []
                            years[y][s].push({ moed: e.moed, index: e.index })
                        }
                        var result = []
                        for (var yi = 0; yi < yearOrder.length; yi++) {
                            var yr = yearOrder[yi]
                            var sems = []
                            for (var sem in years[yr]) {
                                sems.push({ semester: sem, moeds: years[yr][sem] })
                            }
                            result.push({ year: yr, semesters: sems })
                        }
                        return result
                    }

                    delegate: ColumnLayout {
                        width: treeView.width
                        spacing: 2

                        // Year row
                        Rectangle {
                            Layout.fillWidth: true
                            height: 36
                            radius: 8
                            color: treeView.expandedYears[modelData.year]
                                   ? "#2D6A4F" : "transparent"

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 8
                                anchors.rightMargin: 8

                                Text {
                                    text: treeView.expandedYears[modelData.year]
                                          ? "▾" : "▸"
                                    color: "#A8D5B5"
                                    font.pixelSize: 12
                                }
                                Text {
                                    text: modelData.year
                                    color: "white"
                                    font.pixelSize: 14
                                    font.bold: true
                                    Layout.fillWidth: true
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    var exp = Object.assign({}, treeView.expandedYears)
                                    exp[modelData.year] = !exp[modelData.year]
                                    treeView.expandedYears = exp
                                }
                            }
                        }

                        // Semester rows (visible only if year expanded)
                        Repeater {
                            model: treeView.expandedYears[modelData.year]
                                   ? modelData.semesters : []

                            delegate: ColumnLayout {
                                width: treeView.width
                                spacing: 2

                                // Semester row
                                Rectangle {
                                    Layout.fillWidth: true
                                    height: 32
                                    radius: 8
                                    color: treeView.expandedSemesters[modelData.semester]
                                           ? "#3A7D5E" : "transparent"

                                    RowLayout {
                                        anchors.fill: parent
                                        anchors.leftMargin: 20
                                        anchors.rightMargin: 8

                                        Text {
                                            text: treeView.expandedSemesters[modelData.semester]
                                                  ? "▾" : "▸"
                                            color: "#A8D5B5"
                                            font.pixelSize: 11
                                        }
                                        Text {
                                            text: modelData.semester
                                            color: "#D4EDD9"
                                            font.pixelSize: 13
                                            Layout.fillWidth: true
                                        }
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: {
                                            var exp = Object.assign({}, treeView.expandedSemesters)
                                            exp[modelData.semester] = !exp[modelData.semester]
                                            treeView.expandedSemesters = exp
                                        }
                                    }
                                }

                                // Moed leaves (visible only if semester expanded)
                                Repeater {
                                    model: treeView.expandedSemesters[modelData.semester]
                                           ? modelData.moeds : []

                                    delegate: Rectangle {
                                        Layout.fillWidth: true
                                        width: treeView.width
                                        height: 30
                                        radius: 8
                                        color: calendarManager.getPeriodTree().length > modelData.index
                                               && calendarManager.currentSemester ===
                                               (calendarManager.getPeriodTree()[modelData.index].semester
                                                + " - " + calendarManager.getPeriodTree()[modelData.index].moed)
                                               ? "#52B788" : "transparent"

                                        Text {
                                            anchors.verticalCenter: parent.verticalCenter
                                            anchors.left: parent.left
                                            anchors.leftMargin: 36
                                            text: modelData.moed
                                            color: "white"
                                            font.pixelSize: 13
                                        }

                                        MouseArea {
                                            anchors.fill: parent
                                            cursorShape: Qt.PointingHandCursor
                                            onClicked: calendarManager.selectPeriod(modelData.index)
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // ── Right: KPI + calendar ─────────────────────────────────────────────
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            anchors.margins: 24
            spacing: 16

            Item { height: 24 }

            // KPI cards
            RowLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 24
                Layout.rightMargin: 24
                spacing: 12

                Repeater {
                    model: [
                        { label: "Total Days",
                          value: calendarManager.days.length,
                          color: "#1B4332" },
                        { label: "Active Days",
                          value: calendarManager.days.filter(d => d.status === 1).length,
                          color: "#52B788" },
                        { label: "Excluded Days",
                          value: calendarManager.days.filter(d => d.status === 2).length,
                          color: "#C0392B" }
                    ]

                    delegate: Rectangle {
                        Layout.fillWidth: true
                        height: 80
                        radius: 12
                        color: "white"
                        border.color: "#E9ECEF"
                        border.width: 1

                        ColumnLayout {
                            anchors.centerIn: parent
                            spacing: 4

                            Text {
                                text: modelData.value
                                font.pixelSize: 28
                                font.bold: true
                                color: modelData.color
                                Layout.alignment: Qt.AlignHCenter
                            }
                            Text {
                                text: modelData.label
                                font.pixelSize: 12
                                color: "#6C757D"
                                Layout.alignment: Qt.AlignHCenter
                            }
                        }
                    }
                }
            }

            // ── Calendar grid ─────────────────────────────────────────────────
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.leftMargin: 24
                Layout.rightMargin: 24
                Layout.bottomMargin: 24
                radius: 16
                color: "white"
                border.color: "#E9ECEF"
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 8

                    Text {
                        id: monthTitle
                        text: calendarScreen.monthLabel(calendarManager.days)
                        font.pixelSize: 16
                        font.bold: true
                        color: "#1B4332"
                        Layout.alignment: Qt.AlignHCenter
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 4
                        Repeater {
                            model: ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"]
                            delegate: Text {
                                Layout.fillWidth: true
                                text: modelData
                                font.pixelSize: 12
                                font.bold: true
                                color: index === 6 ? "#C0392B" : "#6C757D"
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                    }

                    GridView {
                        id: calGrid
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        cellWidth:  Math.floor(width / 7)
                        cellHeight: 52

                        property var paddedDays: calendarScreen.buildPaddedModel(calendarManager.days)
                        model: paddedDays

                        Connections {
                            target: calendarManager
                            function onDaysChanged() {
                                calGrid.paddedDays = calendarScreen.buildPaddedModel(calendarManager.days)
                                monthTitle.text    = calendarScreen.monthLabel(calendarManager.days)
                            }
                        }

                        delegate: Rectangle {
                            width:  calGrid.cellWidth  - 4
                            height: calGrid.cellHeight - 4
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

                            Canvas {
                                anchors.fill: parent
                                visible: modelData.status === 2
                                onPaint: {
                                    var ctx = getContext("2d")
                                    ctx.clearRect(0, 0, width, height)
                                    ctx.strokeStyle = "#C0392B"
                                    ctx.globalAlpha = 0.15
                                    ctx.lineWidth = 1
                                    for (var i = -height; i < width; i += 8) {
                                        ctx.beginPath()
                                        ctx.moveTo(i, 0)
                                        ctx.lineTo(i + height, height)
                                        ctx.stroke()
                                    }
                                }
                            }

                            ColumnLayout {
                                anchors.centerIn: parent
                                spacing: 2

                                Text {
                                    text: modelData.date
                                          ? Qt.formatDate(modelData.date, "d") : ""
                                    font.pixelSize: 14
                                    font.bold: true
                                    color: modelData.status === 2 ? "#C0392B" : "#1B4332"
                                    Layout.alignment: Qt.AlignHCenter
                                }

                                Text {
                                    text: modelData.label
                                    font.pixelSize: 9
                                    color: "#6C757D"
                                    visible: modelData.label !== ""
                                    Layout.alignment: Qt.AlignHCenter
                                    elide: Text.ElideRight
                                    width: calGrid.cellWidth - 12
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                enabled: modelData.status !== -1
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
}