import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

/*
 * CalendarScreen.qml displays the exam-period calendar review screen.
 *
 * The user can navigate between exam periods, review active/excluded days,
 * toggle available exam days, save changes, and open the period editor screen.
 */
Page {
    id: calendarScreen

    /*
     * Main background color for the calendar screen.
     */
    background: Rectangle { color: "#FAF8F3" }

    // ── Header ──────────────────────────────────────────────────────────────
    /*
     * Top header bar.
     *
     * Contains back navigation, current period title, save action,
     * save confirmation, and navigation to the period editor.
     */
    header: Rectangle {
        width: parent.width
        height: 64
        color: "#1B4332"

        RowLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 12

            /*
             * Back button.
             *
             * Returns the user to the previous screen in the StackView.
             */
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
                onClicked: {
                    if (calendarScreen.StackView.view) {
                        calendarScreen.StackView.view.pop()
                    } else {
                        console.warn("CalendarScreen: StackView view was not found")
                    }
                }
            }

            /*
             * Displays the currently selected semester and moed.
             *
             * The value comes from CalendarManager.
             */
            Text {
                text: calendarManager.currentSemester
                color: "white"
                font.pixelSize: 20
                font.bold: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }

            // Save button in header
            /*
             * Saves the current calendar changes into CalendarManager.
             */
            Button {
                text: "💾 שמור"
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
                    saveConfirm.visible = true
                    saveTimer.restart()
                }
            }

            /*
             * Temporary save confirmation message.
             */
            Text {
                id: saveConfirm
                text: "✅ נשמר"
                color: "white"
                font.pixelSize: 13
                font.bold: true
                visible: false
            }

            /*
             * Hides the save confirmation after two seconds.
             */
            Timer {
                id: saveTimer
                interval: 2000
                onTriggered: saveConfirm.visible = false
            }

            /*
             * Opens the period editor screen.
             */
            Button {
                text: "✏️ ערוך תקופה"
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
                onClicked: {
                    if (calendarScreen.StackView.view) {
                        calendarScreen.StackView.view.push(Qt.resolvedUrl("PeriodEditorScreen.qml"))
                    } else {
                        console.warn("CalendarScreen: StackView view was not found")
                    }
                }
            }
        }
    }

    // ── Helper functions ─────────────────────────────────────────────────────
    /*
     * Adds empty padding days before the first real day.
     *
     * This ensures the calendar grid starts on the correct weekday.
     */
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

    /*
     * Builds the month label shown above the calendar grid.
     */
    function monthLabel(rawDays) {
        if (!rawDays || rawDays.length === 0) return ""
        return Qt.formatDate(rawDays[0].date, "MMMM yyyy")
    }

    // ── Body: sidebar + calendar ─────────────────────────────────────────────
    /*
     * Main screen layout.
     *
     * Left side: exam period navigation tree.
     * Right side: KPI cards and calendar grid.
     */
    RowLayout {
        anchors.fill: parent
        anchors.margins: 0
        spacing: 0

        // ── Left sidebar ─────────────────────────────────────────────────────
        /*
         * Sidebar used to navigate between exam periods.
         */
        Rectangle {
            width: 200
            Layout.fillHeight: true
            color: "#1B4332"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 4

                /*
                 * Sidebar title.
                 */
                Text {
                    text: "תקופות"
                    color: "#A8D5B5"
                    font.pixelSize: 12
                    font.bold: true
                    topPadding: 8
                    bottomPadding: 4
                }

                // Tree built from getPeriodTree()
                /*
                 * Tree view that groups exam periods by year, semester, and moed.
                 */
                ListView {
                    id: treeView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    spacing: 2

                    // Group periods by year then semester
                    /*
                     * Tree data generated from CalendarManager.getPeriodTree().
                     */
                    property var tree: buildTree(calendarManager.getPeriodTree())
                    model: tree

                    // Expanded state: track which years and semesters are open
                    /*
                     * Local expansion state for year and semester nodes.
                     */
                    property var expandedYears: ({})
                    property var expandedSemesters: ({})

                    /*
                     * Converts the flat period list into a nested tree structure.
                     *
                     * The result contains year nodes, semester nodes, and moed leaves.
                     */
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

                    /*
                     * Delegate for each year node in the tree.
                     */
                    delegate: ColumnLayout {
                        width: treeView.width
                        spacing: 2

                        // Year row
                        /*
                         * Clickable year row.
                         *
                         * Expands or collapses the semesters under this year.
                         */
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

                                /*
                                 * Expand/collapse indicator for the year node.
                                 */
                                Text {
                                    text: treeView.expandedYears[modelData.year]
                                          ? "▾" : "▸"
                                    color: "#A8D5B5"
                                    font.pixelSize: 12
                                }

                                /*
                                 * Year label.
                                 */
                                Text {
                                    text: modelData.year
                                    color: "white"
                                    font.pixelSize: 14
                                    font.bold: true
                                    Layout.fillWidth: true
                                }
                            }

                            /*
                             * Toggles year expansion.
                             */
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
                        /*
                         * Semester rows displayed only when the parent year is expanded.
                         */
                        Repeater {
                            model: treeView.expandedYears[modelData.year]
                                   ? modelData.semesters : []

                            delegate: ColumnLayout {
                                width: treeView.width
                                spacing: 2

                                // Semester row
                                /*
                                 * Clickable semester row.
                                 *
                                 * Expands or collapses the moed rows under this semester.
                                 */
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

                                        /*
                                         * Expand/collapse indicator for the semester node.
                                         */
                                        Text {
                                            text: treeView.expandedSemesters[modelData.semester]
                                                  ? "▾" : "▸"
                                            color: "#A8D5B5"
                                            font.pixelSize: 11
                                        }

                                        /*
                                         * Semester label.
                                         */
                                        Text {
                                            text: modelData.semester
                                            color: "#D4EDD9"
                                            font.pixelSize: 13
                                            Layout.fillWidth: true
                                        }
                                    }

                                    /*
                                     * Toggles semester expansion.
                                     */
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
                                /*
                                 * Moed leaf rows displayed only when the semester is expanded.
                                 */
                                Repeater {
                                    model: treeView.expandedSemesters[modelData.semester]
                                           ? modelData.moeds : []

                                    delegate: Rectangle {
                                        Layout.fillWidth: true
                                        width: treeView.width
                                        height: 30
                                        radius: 8

                                        /*
                                         * Highlights the currently selected exam period.
                                         */
                                        color: calendarManager.getPeriodTree().length > modelData.index
                                               && calendarManager.currentSemester ===
                                               (calendarManager.getPeriodTree()[modelData.index].semester
                                                + " - " + calendarManager.getPeriodTree()[modelData.index].moed)
                                               ? "#52B788" : "transparent"

                                        /*
                                         * Moed name.
                                         */
                                        Text {
                                            anchors.verticalCenter: parent.verticalCenter
                                            anchors.left: parent.left
                                            anchors.leftMargin: 36
                                            text: modelData.moed
                                            color: "white"
                                            font.pixelSize: 13
                                        }

                                        /*
                                         * Selecting a moed changes the active period in CalendarManager.
                                         */
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
        /*
         * Right content area containing KPI cards and the editable calendar grid.
         */
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            anchors.margins: 24
            spacing: 16

            Item { height: 24 }

            // KPI cards
            /*
             * Summary cards for total, active, and excluded days.
             */
            RowLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 24
                Layout.rightMargin: 24
                spacing: 12

                /*
                 * Repeated card data.
                 *
                 * Values are calculated directly from calendarManager.days.
                 */
                Repeater {
                    model: [
                        { label: "סך הימים",
                          value: calendarManager.days.length,
                          color: "#1B4332" },
                        { label: "ימים פעילים",
                          value: calendarManager.days.filter(d => d.status === 1).length,
                          color: "#52B788" },
                        { label: "ימים מוחרגים",
                          value: calendarManager.days.filter(d => d.status === 2).length,
                          color: "#C0392B" }
                    ]

                    /*
                     * KPI card delegate.
                     */
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

                            /*
                             * KPI numeric value.
                             */
                            Text {
                                text: modelData.value
                                font.pixelSize: 28
                                font.bold: true
                                color: modelData.color
                                Layout.alignment: Qt.AlignHCenter
                            }

                            /*
                             * KPI label.
                             */
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
            /*
             * Calendar container.
             *
             * Shows the selected period as a grid of days.
             */
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

                    /*
                     * Month title above the grid.
                     */
                    Text {
                        id: monthTitle
                        text: calendarScreen.monthLabel(calendarManager.days)
                        font.pixelSize: 16
                        font.bold: true
                        color: "#1B4332"
                        Layout.alignment: Qt.AlignHCenter
                    }

                    /*
                     * Weekday header row.
                     */
                    Row {
                        Layout.fillWidth: true
                        layoutDirection: Qt.RightToLeft
                        Repeater {
                            model: ["ראשון", "שני", "שלישי", "רביעי", "חמישי", "שישי", "שבת"]
                            delegate: Text {
                                width: Math.floor(calGrid.width / 7)
                                text: modelData
                                font.pixelSize: 14
                                font.bold: true
                                color: "#69737a"
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                    }

                    /*
                     * Calendar grid.
                     *
                     * The model is a padded list so the first day appears under
                     * the correct weekday column.
                     */
                    GridView {
                        id: calGrid
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        cellWidth:  Math.floor(width / 7)
                        cellHeight: 120
                        layoutDirection: Qt.RightToLeft

                        /*
                         * Padded calendar days prepared for grid display.
                         */
                        property var paddedDays: calendarScreen.buildPaddedModel(calendarManager.days)
                        model: paddedDays

                        /*
                         * Refresh the padded grid whenever CalendarManager reports
                         * that the days list changed.
                         */
                        Connections {
                            target: calendarManager
                            function onDaysChanged() {
                                calGrid.paddedDays = calendarScreen.buildPaddedModel(calendarManager.days)
                                monthTitle.text    = calendarScreen.monthLabel(calendarManager.days)
                            }
                        }

                        /*
                         * Delegate for a single calendar cell.
                         */
                        delegate: Rectangle {
                            width:  calGrid.cellWidth  - 10
                            height: calGrid.cellHeight - 10
                            radius: 8
                            visible: modelData.status !== -1

                            /*
                             * Cell background color reflects whether the day is
                             * excluded or not, using the OutputScreen palette.
                             */
                            color: modelData.status === 2 ? "#fef2f2" : "#f1f5f9"

                            /*
                             * Cell border color also reflects the day status,
                             * using the OutputScreen palette.
                             */
                            border.color: modelData.status === 2 ? "#fecaca" : "#e2e8f0"
                            border.width: 1

                            /*
                             * Calendar day number.
                             */
                            Text {
                                anchors.top: parent.top
                                anchors.left: parent.left
                                anchors.margins: 8
                                text: modelData.date
                                      ? Qt.formatDate(modelData.date, "d") : ""
                                font.pixelSize: 14
                                font.bold: true
                                color: modelData.status === 2 ? "#dc2626" : "#64748b"
                            }

                            /*
                             * Clicking an allowed cell toggles the day status.
                             *
                             * Saturday is disabled because it should not be used
                             * as an exam day.
                             */
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
}