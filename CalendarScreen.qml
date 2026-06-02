import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: calendarScreen
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

    // ── Build padded day model (with empty offset cells) ────────────────────
    function buildPaddedModel(rawDays) {
        if (!rawDays || rawDays.length === 0) return []
        // dayOfWeek(): Mon=1 ... Sun=7  →  we want Sun=0 ... Sat=6
        var firstDate = rawDays[0].date
        var dow = firstDate.getDay()          // JS Date: Sun=0 ... Sat=6
        var padded = []
        for (var i = 0; i < dow; i++)
            padded.push({ date: null, status: -1, label: "" })
        for (var j = 0; j < rawDays.length; j++)
            padded.push(rawDays[j])
        return padded
    }

    // ── Current month label ──────────────────────────────────────────────────
    function monthLabel(rawDays) {
        if (!rawDays || rawDays.length === 0) return ""
        var d = rawDays[0].date
        return Qt.formatDate(d, "MMMM yyyy")
    }

    // ── Main Layout ──────────────────────────────────────────────────────────
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        // KPI cards
        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Repeater {
                model: [
                    { label: "Total Days",    value: calendarManager.days.length,
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

        // ── Semester Navigation ──────────────────────────────────────────────
        RowLayout {
            Layout.fillWidth: true

            Button {
                text: "◀ Previous"
                onClicked: calendarManager.previousSemester()
                background: Rectangle {
                    radius: 8
                    color: parent.hovered ? "#E9ECEF" : "white"
                    border.color: "#1B4332"
                    border.width: 1
                }
                contentItem: Text {
                    text: parent.text
                    color: "#1B4332"
                    font.pixelSize: 13
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Item { Layout.fillWidth: true }

            Button {
                text: "Next ▶"
                onClicked: calendarManager.nextSemester()
                background: Rectangle {
                    radius: 8
                    color: parent.hovered ? "#E9ECEF" : "white"
                    border.color: "#1B4332"
                    border.width: 1
                }
                contentItem: Text {
                    text: parent.text
                    color: "#1B4332"
                    font.pixelSize: 13
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        // ── Calendar Grid ────────────────────────────────────────────────────
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

                // Month label
                Text {
                    id: monthTitle
                    text: calendarScreen.monthLabel(calendarManager.days)
                    font.pixelSize: 16
                    font.bold: true
                    color: "#1B4332"
                    Layout.alignment: Qt.AlignHCenter
                }

                // Day-of-week headers
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
                            // Highlight Saturday header in red
                            color: index === 6 ? "#C0392B" : "#6C757D"
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }

                // Calendar days grid
                GridView {
                    id: calGrid
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    cellWidth:  Math.floor(width / 7)
                    cellHeight: 52

                    // Rebuild padded model whenever days change
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

                        // Empty offset cell
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

                        // Striped overlay for excluded days
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
                                text: modelData.date ? Qt.formatDate(modelData.date, "d") : ""
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