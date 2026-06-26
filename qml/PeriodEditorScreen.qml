import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

/*
 * PeriodEditorScreen.qml allows the user to edit the currently selected
 * exam period.
 *
 * The user can:
 * - change the start and end dates of the period
 * - apply the shifted date range
 * - save changes
 * - toggle individual days between active and excluded
 */
Page {
    id: editorScreen

    /*
     * Main background color for the period editor screen.
     */
    background: Rectangle { color: "#FAF8F3" }

    /*
     * Header bar for navigation and title display.
     */
    header: Rectangle {
        width: parent.width
        height: 64
        color: "#1B4332"

        RowLayout {
            anchors.fill: parent
            anchors.margins: 16

            /*
             * Back button.
             *
             * Returns to the previous screen in the StackView.
             */
            Button {
                text: "← חזור"
                flat: true
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.pixelSize: 15
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle { color: "transparent" }
                onClicked: {
                    if (editorScreen.StackView.view) {
                        editorScreen.StackView.view.pop()
                    } else {
                        console.warn("PeriodEditorScreen: StackView view was not found")
                    }
                }
            }

            /*
             * Screen title showing the currently selected semester and moed.
             */
            Text {
                text: "עריכת תקופת בחינות — " + calendarManager.currentSemester
                color: "white"
                font.pixelSize: 18
                font.bold: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    /*
     * Initialize the date fields when the editor screen is loaded.
     */
    Component.onCompleted: {
        startDateField.text = calendarManager.currentStartDate
        endDateField.text   = calendarManager.currentEndDate
    }

    /*
     * Keep the date fields synchronized when the selected period changes.
     */
    Connections {
        target: calendarManager
        function onSemesterChanged() {
            startDateField.text = calendarManager.currentStartDate
            endDateField.text   = calendarManager.currentEndDate
        }
    }

    /*
     * Adds padding cells before the first real date.
     *
     * This keeps the calendar aligned with the correct weekday column.
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
     * Returns the month label displayed above the calendar grid.
     */
    function monthLabel(rawDays) {
        if (!rawDays || rawDays.length === 0) return ""
        return Qt.formatDate(rawDays[0].date, "MMMM yyyy")
    }

    /*
     * Main layout of the period editor.
     */
    ScrollView {
        id: contentScroll
        anchors.fill: parent
        anchors.margins: 24
        clip: true
        contentWidth: availableWidth   // content never scrolls sideways

        ColumnLayout {
            id: scrollContent
            width: contentScroll.availableWidth
            // Fill the viewport on large windows (identical layout), but grow
            // taller on small ones so the page scroll bar appears.
            height: Math.max(implicitHeight, contentScroll.availableHeight)
            spacing: 20

        // Date range inputs + buttons
        /*
         * Date range editing panel.
         *
         * Contains start/end date fields, apply-shift action, save action,
         * and short success/error messages.
         */
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 120
            Layout.minimumHeight: 120
            radius: 12
            color: "white"
            border.color: "#E9ECEF"
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 20

                /*
                 * Start date input.
                 *
                 * Expected format: YYYY-MM-DD.
                 */
                ColumnLayout {
                    spacing: 6
                    Text { text: "תאריך התחלה"; font.pixelSize: 13; color: "#6C757D" }
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

                /*
                 * End date input.
                 *
                 * Expected format: YYYY-MM-DD.
                 */
                ColumnLayout {
                    spacing: 6
                    Text { text: "תאריך סיום"; font.pixelSize: 13; color: "#6C757D" }
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
                /*
                 * Applies the new period date range.
                 *
                 * The input is validated before calling CalendarManager.shiftPeriod().
                 */
                Button {
                    text: "החל הזזה"
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
                        var isoDateRegex = /^\d{4}-\d{2}-\d{2}$/

                        /*
                         * Validate ISO date format and make sure the start date
                         * is not later than the end date.
                         */
                        if (!isoDateRegex.test(startDateField.text) ||
                            !isoDateRegex.test(endDateField.text) ||
                            startDateField.text > endDateField.text) {
                            shiftErrorMsg.visible = true
                            shiftErrorTimer.restart()
                            return
                        }

                        /*
                         * Ask the C++ CalendarManager to update the selected period.
                         */
                        calendarManager.shiftPeriod(
                            calendarManager.currentSemester,
                            startDateField.text,
                            endDateField.text
                        )

                        /*
                         * Show a temporary success message.
                         */
                        saveConfirm.text = "✅ ההזזה הוחלה"
                        saveConfirm.visible = true
                        saveTimer.restart()
                    }
                }

                // Save button
                /*
                 * Saves the current period changes.
                 */
                Button {
                    text: "💾 שמור"
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
                /*
                 * Temporary success message shown after saving or applying a shift.
                 */
                Text {
                    id: saveConfirm
                    text: "✅ נשמר"
                    color: "#157347"
                    font.pixelSize: 13
                    font.bold: true
                    visible: false
                }

                /*
                 * Hides the success message after two seconds.
                 */
                Timer {
                    id: saveTimer
                    interval: 2000
                    onTriggered: saveConfirm.visible = false
                }

                // Error message for invalid date range
                /*
                 * Error message shown when the entered date range is invalid.
                 */
                Text {
                    id: shiftErrorMsg
                    text: "⚠ שגיאה: תאריך ההתחלה מאוחר מתאריך הסיום"
                    color: "#C0392B"
                    font.pixelSize: 13
                    font.bold: true
                    visible: false
                }

                /*
                 * Hides the error message after three seconds.
                 */
                Timer {
                    id: shiftErrorTimer
                    interval: 3000
                    onTriggered: shiftErrorMsg.visible = false
                }
            }
        }

        // Calendar grid
        /*
         * Editable calendar grid.
         *
         * The user can click a valid day to exclude or restore it.
         */
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 320 // keep the calendar usable once the page scrolls
            radius: 16
            color: "white"
            border.color: "#E9ECEF"
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 8

                /*
                 * Instruction text for the user.
                 */
                Text {
                    text: "לחץ על יום כדי להחריג / לשחזר אותו"
                    font.pixelSize: 13
                    color: "#6C757D"
                    font.italic: true
                }

                /*
                 * Month title above the calendar grid.
                 */
                Text {
                    id: editorMonthTitle
                    text: editorScreen.monthLabel(calendarManager.days)
                    font.pixelSize: 15
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
                            width: Math.floor(editorGrid.width / 7)
                            text: modelData
                            font.pixelSize: 14
                            font.bold: true
                            color: "#69737a"
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }

                /*
                 * Calendar grid view.
                 *
                 * The grid uses paddedDays so the first day appears under the
                 * correct weekday.
                 */
                /*
                 * Shared 7-column calendar grid (CalendarGrid.qml).
                 *
                 * Rendering, cell size, RTL and the per-day toggle behaviour are
                 * identical to the previous inline grid; the state colors come
                 * from the component's defaults, which match the old hardcoded
                 * values exactly.
                 */
                CalendarGrid {
                    id: editorGrid

                    /*
                     * Padded model built from CalendarManager.days, refreshed (with
                     * the month title) whenever the day list changes — unchanged.
                     */
                    property var paddedDays: editorScreen.buildPaddedModel(calendarManager.days)
                    model: paddedDays

                    /*
                     * Toggle a day exactly as before: forward the clicked date to
                     * CalendarManager. Saturday/padding cells are filtered inside
                     * CalendarGrid, so this only fires for togglable days.
                     */
                    onDayToggled: (dateStr) => calendarManager.toggleDay(dateStr)

                    Connections {
                        target: calendarManager
                        function onDaysChanged() {
                            editorGrid.paddedDays = editorScreen.buildPaddedModel(calendarManager.days)
                            editorMonthTitle.text = editorScreen.monthLabel(calendarManager.days)
                        }
                    }
                }
            }
        }
        } // scrollContent
    } // contentScroll
}