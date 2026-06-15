import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

/*
 * OutputScreen.qml displays generated exam schedules.
 *
 * The screen allows the user to:
 * - navigate between generated schedules
 * - filter the shown calendar by semester and moed
 * - view exams inside a calendar grid
 * - export the currently selected schedule to a text file
 * - return to the input screen
 */
Item {
    id: outputRoot

    // dialog for choosing where to save the exported schedule
    /*
     * File dialog used for exporting the current schedule.
     *
     * The selected file path is passed to ScheduleOutputManager, which performs
     * validation and writes the schedule to a text file.
     */
    FileDialog {
        id: saveDialog
        title: "בחר היכן לשמור את המערכת"
        fileMode: FileDialog.SaveFile
        nameFilters: ["Text files (*.txt)"]
        
        /*
         * When the user confirms the save location, convert the selected URL
         * into a local file path and request the C++ output manager to export.
         */
        onAccepted: {
            let path = saveDialog.selectedFile.toString().replace("file:///", "")
            
            if (appController.outputManager.saveCurrentScheduleToFile(path)) {
                console.log("File saved successfully to: " + path)
            } else {
                console.log("Failed to save!")
            }
        }
    }


    /*
     * Main vertical layout of the output screen.
     */
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 34
        spacing: 16

        // export + back buttons
        /*
         * Top action bar.
         *
         * Contains the back button on the right side and the export button
         * on the left side.
         */
        RowLayout {
            Layout.fillWidth: true
            layoutDirection: Qt.RightToLeft

            /*
             * Back button.
             *
             * Returns to the previous screen and clears the output manager data.
             */
            Button {
                text: "← חזור לקלט נתונים"
                font.pixelSize: 16
                font.bold: true
                background: Rectangle {
                    color: parent.down ? "#e2e8f0" : (parent.hovered ? "#f1f5f9" : "transparent")
                    radius: 8
                }
                onClicked: {
                    if (outputRoot.StackView.view) {
                        outputRoot.StackView.view.pop()
                        appController.outputManager.clearData()
                    } else {
                        console.log("OutputScreen: StackView view was not found")
                    }
                }
            }

            Item { Layout.fillWidth: true } // pushes the export button to the left

            // export button
            /*
             * Export button.
             *
             * Enabled only when at least one generated schedule exists and
             * the current schedule index is valid.
             */
            Button {
                text: "📥 ייצא מערכת זו"
                font.pixelSize: 15
                font.bold: true
                Layout.preferredWidth: 150
                Layout.preferredHeight: 40
                enabled: appController.outputManager.totalSchedulesCount > 0 &&
                appController.outputManager.currentScheduleIndex >= 1

                background: Rectangle {
                    color: !parent.enabled ? "#94a3b8" : (parent.down ? "#0f3f30" : (parent.hovered ? "#1b664f" : "#14533f"))
                    radius: 8
                }
                
                /*
                 * Custom text item for consistent button styling.
                 */
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.bold: true
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                /*
                 * Opens the save dialog so the user can choose an export location.
                 */
                onClicked: {
                    console.log("Exporting schedule " + outputRoot.currentScheduleIndex)
                    saveDialog.open()
                }
            }
        }

        // navigation between different schedules (if multiple were generated from the input)
        /*
         * Schedule navigation bar.
         *
         * Allows the user to move between multiple generated schedule solutions.
         */
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

                /*
                 * Previous schedule button.
                 */
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

                    /*
                     * Requests the C++ output manager to move to the previous solution.
                     */
                    onClicked: {
                        appController.outputManager.previousSchedule();
                    }
                }

                /*
                 * Current schedule position label.
                 */
                Text {
                    Layout.fillWidth: true
                    text: "מערכת " + appController.outputManager.currentScheduleIndex + " מתוך " + appController.outputManager.totalSchedulesCount
                    font.pixelSize: 18
                    font.bold: true
                    color: "#1f2933"
                    horizontalAlignment: Text.AlignHCenter
                }

                /*
                 * Next schedule button.
                 */
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

                    /*
                     * Requests the C++ output manager to move to the next solution.
                     */
                    onClicked: {
                        appController.outputManager.nextSchedule();
                    }
                }
            }
        }

        // choice of semester/moed for filtering the calendar 
        /*
         * Semester and moed filter row.
         *
         * Changing either filter regenerates the displayed output for the
         * selected period.
         */
        RowLayout {
            Layout.fillWidth: true
            layoutDirection: Qt.RightToLeft
            spacing: 12
            Layout.topMargin: 8
            Layout.bottomMargin: 4

            Item { Layout.fillWidth: true } 

            /*
             * Semester filter label.
             */
            Text {
                text: "סמסטר:"
                font.pixelSize: 16
                font.bold: true
                color: "#1f2933"
            }

            /*
             * Semester selector.
             *
             * The model comes from ScheduleOutputManager in C++.
             */
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
                
                /*
                 * Generate schedules for the newly selected semester and current moed.
                 */
                onActivated: {
                    appController.generateForPeriod(currentText, moedSelector.currentText);
                }
            }

            Item { width: 20 } 

            /*
             * Moed filter label.
             */
            Text {
                text: "מועד:"
                font.pixelSize: 16
                font.bold: true
                color: "#1f2933"
            }

            /*
             * Moed selector.
             *
             * The model comes from ScheduleOutputManager in C++.
             */
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
                
                /*
                 * Generate schedules for the current semester and newly selected moed.
                 */
                onActivated: {
                    appController.generateForPeriod(semesterSelector.currentText, currentText);
                }
            }

            Item { Layout.fillWidth: true }
        }

        // Calendar grid
        /*
         * Main calendar output container.
         *
         * Displays the selected generated schedule in a weekly calendar layout.
         */
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 16
            color: "white"
            border.color: "#e1e5df"
            border.width: 1
            clip: true

            /*
             * Weekday header area.
             */
            Item {
                id: weekDaysHeader
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 20
                height: 30

                /*
                 * Hebrew weekday labels.
                 */
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

            /*
             * Grid view containing calendar day cells.
             *
             * The data model is prepared by ScheduleOutputManager as QVariantList.
             */
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

                /*
                 * Vertical scrollbar for long calendar ranges.
                 */
                ScrollBar.vertical: ScrollBar {
                    active: true
                    policy: ScrollBar.AsNeeded
                }

                /*
                 * Delegate for one calendar cell.
                 *
                 * A cell can be:
                 * - empty padding cell
                 * - excluded day
                 * - regular day without an exam
                 * - day with an exam
                 */
                delegate: Rectangle {
                    width: calendarGrid.cellWidth - 10
                    height: calendarGrid.cellHeight - 10
                    radius: 8
                    
                    /*
                     * Background color depends on whether the day is empty,
                     * excluded, contains an exam, or is a regular day.
                     */
                    color: modelData.dayText === "" ? "transparent" : (modelData.isExcluded ? "#fef2f2" : (modelData.hasExam ? "#f8fafc" : "#f1f5f9"))

                    /*
                     * Border color highlights excluded days and exam requirement type.
                     */
                    border.color: modelData.dayText === "" ? "transparent" : (modelData.isExcluded ? "#fecaca" : (modelData.hasExam ? (modelData.req === "חובה" ? "#fca5a5" : "#86efac") : "#e2e8f0"))
                    border.width: 1

                    /*
                     * Day number text.
                     */
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

                    /*
                     * Exam details shown only when the current day has an exam.
                     */
                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        anchors.topMargin: 24
                        spacing: 2
                        visible: modelData.hasExam === true

                        /*
                         * Exam course name.
                         */
                        Text {
                            Layout.fillWidth: true
                            text: modelData.examName || ""
                            font.pixelSize: 13
                            font.bold: true
                            color: "#1f2933"
                            elide: Text.ElideRight
                            horizontalAlignment: Text.AlignRight
                        }

                        /*
                         * Course ID and requirement type.
                         */
                        Text {
                            Layout.fillWidth: true
                            text: (modelData.courseId || "") + " | " + (modelData.req || "")
                            font.pixelSize: 11
                            color: modelData.req === "חובה" ? "#b91c1c" : "#047857"
                            horizontalAlignment: Text.AlignRight
                        }

                        /*
                         * Program name or ID.
                         */
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

            // no solutions message
            /*
             * Empty-state message.
             *
             * Shown when the scheduling process did not produce any valid schedules.
             */
            Rectangle {
                anchors.centerIn: parent
                width: 350
                height: 80
                radius: 8
                color: "#fef2f2"
                border.color: "#f87171"
                border.width: 1

                // only visible when there is no solutions
                visible: appController.outputManager.totalSchedulesCount === 0

                Text {
                    anchors.centerIn: parent
                    text: "לא נמצאו מערכות שיבוץ תקינות לנתונים אלו."
                    font.pixelSize: 16
                    font.bold: true
                    color: "#991b1b"
                }
            }
        }
    }
}