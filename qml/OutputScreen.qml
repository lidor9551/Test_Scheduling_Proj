import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQml.Models

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

    /**
     * Local color palette.
     *
     * Components defined in Main.qml (AppButton, FileCard, the root palette) are
     * out of scope from a screen pushed onto the StackView, so the colors used
     * here are redeclared locally, exactly as SettingsScreen.qml does.
     */
    readonly property color primary:     "#14533f"
    readonly property color primaryDark: "#0f3f30"
    readonly property color borderSoft:  "#e1e5df"
    readonly property color textDark:    "#1f2933"
    readonly property color textMuted:   "#69737a"

    /**
     * Whether the left sorting sidebar is expanded.
     *
     * Toggled by the "מיון" button under the "next" navigation button. When true
     * the sidebar animates open and pushes only the calendar grid to the right;
     * when false the sidebar collapses to zero width and the calendar reclaims
     * the full width. The bars above the calendar never move.
     */
    property bool sidebarOpen: false

    /**
     * Maps every sorting metric identifier to its Hebrew label.
     *
     * The identifiers mirror the hard constraint keys ("2.1".."2.5"), so a
     * sorting metric is only relevant when its matching hard constraint is
     * enabled. All metrics rank schedules in descending order.
     */
    readonly property var metricLabels: ({
        "metric_avgDaysObligatory": "ממוצע ימים בין מבחני חובה (אותה שנה)",
        "metric_avgDaysAll": "ממוצע ימים בין כלל המבחנים (חובה או בחירה)",
        "metric_electiveConflicts": "מספר ההתנגשויות המינימלי בין קורסי בחירה",
        "metric_obligatorySpan": "הפער (בימים) בין בחינת החובה הראשונה לאחרונה",
        "metric_maxExamsPerDay": "מספר הבחינות המקסימלי שנערך באותו יום (לכלל המוסד)"
    })

    /**
     * Backing model for the drag & drop priority list.
     *
     * Populated once in Component.onCompleted from the enabled hard constraints
     * and the previously saved order. The visual reordering happens on the
     * DelegateModel (priorityVisualModel), never on this model directly, so the
     * drag stays local until the user presses Save.
     */
    ListModel {
        id: priorityModel
    }

    /**
     * Builds the priority list shown to the user.
     *
     * Keeps only the metrics whose matching hard constraint is enabled, then
     * applies the saved order from getSortingPriorities() when present. Any
     * enabled metric missing from the saved order is appended at the end so a
     * newly enabled constraint still appears. Writes to priorityModel only here,
     * on load — never from a binding — to avoid binding loops.
     */
    function rebuildPriorityModel() {
        // These are the exact metric IDs the C++ engine listens for
        var allIds = [
            "metric_avgDaysObligatory", 
            "metric_avgDaysAll", 
            "metric_electiveConflicts", 
            "metric_obligatorySpan", 
            "metric_maxExamsPerDay"
        ]

        // Apply the previously saved order, keeping only still-enabled metrics.
        var saved = appController.getSortingPriorities()
        var ordered = []
        // Apply the previously saved order first
        if (saved && saved.length > 0) {
            for (var s = 0; s < saved.length; ++s) {
                if (allIds.indexOf(saved[s]) !== -1 && ordered.indexOf(saved[s]) === -1) {
                    ordered.push(saved[s])
                }
            }
        }
        
        // Append any metrics that were not part of the saved order 
        // (useful for first-time load or if a new metric was added to the system)
        for (var e = 0; e < allIds.length; ++e) {
            if (ordered.indexOf(allIds[e]) === -1) {
                ordered.push(allIds[e])
            }
        }

        priorityModel.clear()
        for (var k = 0; k < ordered.length; ++k) {
            priorityModel.append({
                metricId: ordered[k],
                label:    outputRoot.metricLabels[ordered[k]]
            })
        }
    }

    /**
     * Collects the current visual order and persists it to the backend.
     *
     * Reads the order straight from the DelegateModel (which reflects the drag
     * reordering) rather than priorityModel, builds a QVariantList of metric
     * identifiers, and forwards it to AppController.saveSortingPriorities().
     */
    function saveCurrentOrder() {
        var orderedIds = []
        for (var i = 0; i < priorityVisualModel.items.count; ++i) {
            orderedIds.push(priorityVisualModel.items.get(i).model.metricId)
        }
        appController.saveSortingPriorities(orderedIds)
    }

    Component.onCompleted: outputRoot.rebuildPriorityModel()

    /**
     * Pure-QML mock of the future drag & drop backend.
     *
     * Simulates the scheduling service the real C++ layer will expose later. The
     * validity rule mirrors a real scheduling constraint using data already in
     * the model: a date is droppable only when its calendar cell is not excluded
     * (excluded days are holidays / shabbat where no exam may be placed). This
     * object is the single source of truth for the two API methods the drag &
     * drop layer calls, matching the agreed contract exactly.
     */
    QtObject {
        id: mockScheduleManager

        function getValidDatesForCourse(courseId) {
            // This now returns the real validity map from C++
            return appController.outputManager.getValidDatesForCourse(courseId);
        }

       function requestMove(courseId, newDate) {
            // Call the C++ backend function we just implemented
            // Make sure 'appController.outputManager' is accessible in your QML
            var response = appController.outputManager.requestMove(courseId, newDate);

             // The C++ logic handled all the complex rule checks (Adapter + RulesEngine)
            if (response.status === 1) {
                console.log("Move successful! C++ validated it.");
                return { "status": 1 };
            } else {
                // Here we handle the failure
                console.log("Move rejected by C++ engine. Error: " + response.error);
        
            // Use this to show a popup to the user in QML
            // showErrorDialog("Move rejected: " + response.error);
        
            return { "status": 0, "error": response.error };
            }
        }
    }

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
        nameFilters: ["קבצי טקסט (*.txt)"]

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

    /**
     * Reusable drag & drop priority list, shared by the sidebar.
     *
     * Defined once at the root scope so the DelegateModel/ListView keep a single
     * identity regardless of where the sidebar lives in the tree.
     *
     * DelegateModel wrapping priorityModel: the drag reordering moves items on
     * this visual model only (items.move), leaving priorityModel untouched until
     * the user saves. The rank shown next to each row is derived from
     * DelegateModel.itemsIndex, which updates live during the drag.
     */
    DelegateModel {
        id: priorityVisualModel
        model: priorityModel

        delegate: Item {
            id: delegateRoot
            width: priorityList.width
            // Adaptive height: grows with the wrapped label, never below 56 px.
            height: Math.max(56, labelText.implicitHeight + 24)

            /** True while this row is being dragged (long-press engaged). */
            property bool held: false

            /**
             * Reorders the visual model as a dragged row hovers over this one.
             * Only the DelegateModel order changes here.
             */
            DropArea {
                anchors.fill: parent
                onEntered: function(drag) {
                    priorityVisualModel.items.move(
                        drag.source.DelegateModel.itemsIndex,
                        delegateRoot.DelegateModel.itemsIndex)
                }
            }

            Rectangle {
                id: rowContent
                width: delegateRoot.width
                height: delegateRoot.height - 8
                radius: 10
                color: delegateRoot.held ? "#edf7f2" : "#f8fafc"
                border.color: delegateRoot.held ? outputRoot.primary
                                                : outputRoot.borderSoft
                border.width: delegateRoot.held ? 2 : 1

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter

                // Required so the dragged row floats above its peers.
                Drag.active: delegateRoot.held
                Drag.source: delegateRoot
                Drag.hotSpot.x: width / 2
                Drag.hotSpot.y: rowContent.height / 2

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    anchors.rightMargin: 12
                    layoutDirection: Qt.RightToLeft
                    spacing: 10

                    /** Priority rank badge (1 = highest). */
                    Rectangle {
                        width: 26
                        height: 26
                        radius: 8
                        color: outputRoot.primary
                        Text {
                            anchors.centerIn: parent
                            text: delegateRoot.DelegateModel.itemsIndex + 1
                            color: "white"
                            font.pixelSize: 13
                            font.bold: true
                        }
                    }

                    /** Metric Hebrew label. */
                    Text {
                        id: labelText
                        Layout.fillWidth: true
                        text: model.label
                        font.pixelSize: 12
                        color: outputRoot.textDark
                        horizontalAlignment: Text.AlignRight
                        wrapMode: Text.WordWrap
                    }

                    /**
                     * Drag handle on the left side. Purely a visual affordance
                     * now: the whole card is draggable via long press below.
                     */
                    Text {
                        text: "≡"
                        font.pixelSize: 22
                        color: outputRoot.textMuted
                    }
                }

                /**
                 * Whole-card drag area.
                 *
                 * A long press (~300 ms) anywhere on the card engages the drag,
                 * so a short tap never triggers an accidental reorder. The press
                 * timer sets held=true, which switches on drag.target and lets the
                 * still-pressed pointer drag the row. Covers the full card, the ≡
                 * handle included, keeping the handle as a visual hint only.
                 */
                MouseArea {
                    id: dragArea
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    drag.target: delegateRoot.held ? rowContent : undefined
                    drag.axis: Drag.YAxis

                    /** Engages the drag once the press is held long enough. */
                    Timer {
                        id: pressTimer
                        interval: 300
                        onTriggered: delegateRoot.held = true
                    }

                    onPressed: pressTimer.start()
                    onReleased: {
                        pressTimer.stop()
                        delegateRoot.held = false
                    }
                    onCanceled: {
                        pressTimer.stop()
                        delegateRoot.held = false
                    }
                }
            }

            /**
             * While held, reparent the row to the ListView so it can float
             * freely over the other rows during the drag.
             */
            states: State {
                when: delegateRoot.held
                ParentChange {
                    target: rowContent
                    parent: priorityList
                }
                AnchorChanges {
                    target: rowContent
                    anchors.horizontalCenter: undefined
                    anchors.verticalCenter: undefined
                }
            }
        }
    }

    /**
     * Main vertical layout of the output screen.
     *
     * The top bars (action bar, schedule navigation, semester/moed filters) stay
     * full width and never move. Only the bottom row — where the sorting sidebar
     * sits beside the calendar — reacts to the sidebar opening.
     */
    ColumnLayout {
        id: mainContent
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
            Layout.preferredHeight: 60
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
                    Layout.alignment: Qt.AlignVCenter
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
                    Layout.alignment: Qt.AlignVCenter
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

            // sorting sidebar toggle
            /**
             * Toggle button for the left sorting sidebar.
             *
             * Last child of the RTL filter row, so it sits flush against the left
             * edge. Only shown once schedules exist, since the priority order only
             * matters for ranking real schedules.
             */
            Button {
                text: "מיון ⚙️"
                font.pixelSize: 15
                font.bold: true
                Layout.preferredWidth: 120
                Layout.preferredHeight: 40
                visible: appController.outputManager.totalSchedulesCount > 0

                background: Rectangle {
                    color: outputRoot.sidebarOpen ? outputRoot.primaryDark
                         : parent.down            ? "#e2e8f0"
                         : parent.hovered          ? "#f1f5f9"
                         : "white"
                    border.color: "#cbd5e1"
                    border.width: 1
                    radius: 8
                }
                contentItem: Text {
                    text: parent.text
                    color: outputRoot.sidebarOpen ? "white" : outputRoot.textDark
                    font.pixelSize: 15
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: outputRoot.sidebarOpen = !outputRoot.sidebarOpen
            }
        }

        // ---------------------------------------------------------------------
        // Bottom row: [ sorting sidebar | calendar grid ].
        // ---------------------------------------------------------------------
        /**
         * Bottom horizontal area pairing the sorting sidebar with the calendar.
         *
         * Only this row reacts to the sidebar: when it opens, the sidebar takes
         * its 340 px and the calendar (Layout.fillWidth) shrinks to fit, leaving
         * the bars above untouched and producing no empty white space.
         */
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: outputRoot.sidebarOpen ? 16 : 0

            /**
             * Collapsible sorting sidebar.
             *
             * Holds the full sorting metrics prioritization panel. Its width is
             * animated so the open/close feels fluid; clip keeps the content
             * hidden while collapsed. Sits at the calendar's exact height since
             * it shares this fill-height row.
             */
            Rectangle {
                id: sidebar
                Layout.fillHeight: true
                Layout.preferredWidth: outputRoot.sidebarOpen ? 340 : 0
                clip: true
                color: "white"
                border.color: outputRoot.borderSoft
                border.width: outputRoot.sidebarOpen ? 1 : 0
                radius: 16

                /** Smooth width animation driving the Gemini-style push effect. */
                Behavior on Layout.preferredWidth {
                    NumberAnimation { duration: 220; easing.type: Easing.InOutQuad }
                }

                /**
                 * Scrollable container so a long metric list never overflows the
                 * sidebar; the list scrolls instead of being clipped.
                 */
                ScrollView {
                    anchors.fill: parent
                    anchors.margins: 16
                    clip: true
                    contentWidth: availableWidth

                    ColumnLayout {
                        id: priorityColumn
                        width: parent.width
                        spacing: 12

                        /** Sidebar header row: title + close button. */
                        RowLayout {
                            Layout.fillWidth: true
                            layoutDirection: Qt.RightToLeft
                            spacing: 8

                            /** Panel title. */
                            Text {
                                Layout.fillWidth: true
                                text: "סדר עדיפויות למיון מערכות"
                                font.pixelSize: 18
                                font.bold: true
                                color: outputRoot.textDark
                                horizontalAlignment: Text.AlignRight
                                wrapMode: Text.WordWrap
                            }

                            /** Close button — collapses the sidebar. */
                            Button {
                                text: "✕"
                                font.pixelSize: 16
                                implicitWidth: 32
                                implicitHeight: 32
                                background: Rectangle {
                                    color: parent.down ? "#e2e8f0"
                                         : parent.hovered ? "#f1f5f9"
                                         : "transparent"
                                    radius: 8
                                }
                                onClicked: outputRoot.sidebarOpen = false
                            }
                        }

                        /** Short instruction line. */
                        Text {
                            Layout.fillWidth: true
                            text: "גרור את הקריטריונים כדי לקבוע את סדר העדיפויות (גבוה לנמוך)"
                            font.pixelSize: 13
                            color: outputRoot.textMuted
                            horizontalAlignment: Text.AlignRight
                            wrapMode: Text.WordWrap
                        }

                        /**
                         * Empty-state message shown when no hard constraint is
                         * enabled, so there is no sorting metric to prioritize.
                         */
                        Text {
                            Layout.fillWidth: true
                            visible: priorityModel.count === 0
                            text: "לא הופעלו קריטריונים למיון"
                            font.pixelSize: 14
                            font.bold: true
                            color: outputRoot.textMuted
                            horizontalAlignment: Text.AlignRight
                        }

                        /** Draggable, reorderable list of the enabled sorting metrics. */
                        ListView {
                            id: priorityList
                            Layout.fillWidth: true
                            Layout.preferredHeight: contentHeight
                            visible: priorityModel.count > 0
                            interactive: false
                            spacing: 6
                            model: priorityVisualModel
                        }

                        /** Saves the current priority order to the backend. */
                        Button {
                            Layout.fillWidth: true
                            visible: priorityModel.count > 0
                            text: "שמור סדר עדיפויות"
                            font.pixelSize: 15
                            font.bold: true
                            implicitHeight: 42
                            padding: 14

                            background: Rectangle {
                                color: parent.down    ? outputRoot.primaryDark
                                     : parent.hovered ? "#1b664f"
                                     : outputRoot.primary
                                radius: 8
                            }
                            contentItem: Text {
                                text: parent.text
                                color: "white"
                                font.pixelSize: 15
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            /**
                             * Persists the order, then collapses the sidebar so
                             * the calendar reclaims the full width right away.
                             */
                            onClicked: {
                                outputRoot.saveCurrentOrder()
                                outputRoot.sidebarOpen = false
                            }
                        }
                    }
                }
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

                    /**
                     * Drag & drop shared state, read by every day cell.
                     *
                     * dragActive toggles the valid/invalid drop overlays;
                     * validDates holds the { "dd-MM-yyyy": bool } map for the card
                     * currently dragged; draggedCourseId keeps that card's course.
                     */
                    property bool dragActive: false
                    property var validDates: ({})
                    property string draggedCourseId: ""

                    cellWidth: width / 7

                    /**
                     * Largest number of exams falling on a single day across the
                     * whole calendar. Recomputed whenever the data changes so the
                     * uniform cell height can grow to fit the busiest day.
                     */
                    property int maxExamsPerDay: {
                        var data = appController.outputManager.currentCalendarData
                        var max = 1
                        if (data) {
                            for (var i = 0; i < data.length; ++i) {
                                var exams = data[i].exams
                                if (exams && exams.length > max) {
                                    max = exams.length
                                }
                            }
                        }
                        return max
                    }

                    /**
                     * Uniform cell height. GridView lays cells on a fixed grid,
                     * so every cell shares this height; it is sized for the
                     * busiest day and never drops below the original 120 px
                     * single-exam height.
                     *
                     * Budget: 24 px day-number band, ~64 px per exam block
                     * (three text lines + internal padding), plus ~16 px per gap
                     * (column spacing + separator) between consecutive exams.
                     */
                    cellHeight: Math.max(120,
                                         24
                                         + calendarGrid.maxExamsPerDay * 64
                                         + Math.max(0, calendarGrid.maxExamsPerDay - 1) * 16)

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
                        id: dayCell

                        /**
                         * Number of exams scheduled on this day.
                         */
                        readonly property int examCount:
                            (modelData.exams ? modelData.exams.length : 0)

                        /**
                         * True when at least one exam on this day is obligatory
                         * ("חובה"). Drives the requirement-based border color now
                         * that a day may hold several exams of mixed types.
                         */
                        readonly property bool hasObligatory: {
                            var exams = modelData.exams
                            if (exams) {
                                for (var i = 0; i < exams.length; ++i) {
                                    if (exams[i].req === "חובה") {
                                        return true
                                    }
                                }
                            }
                            return false
                        }

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
                        border.color: modelData.dayText === "" ? "transparent" : (modelData.isExcluded ? "#fecaca" : (modelData.hasExam ? (dayCell.hasObligatory ? "#fca5a5" : "#86efac") : "#e2e8f0"))
                        border.width: 1

                        /**
                         * Drop-target overlay shown only while a drag is in
                         * progress and only on real (non-padding) cells. Green
                         * marks a valid drop date, red an invalid one. It carries
                         * no MouseArea so it never intercepts the drag pointer.
                         */
                        Rectangle {
                            anchors.fill: parent
                            z: 5
                            radius: 4
                            opacity: 0.25
                            visible: calendarGrid.dragActive && modelData.dateKey !== ""
                            color: {
                                if (!calendarGrid.validDates) return "#fca5a5"; 
                                
                                // Debugging: Print keys only once to console to avoid spam
                                console.log("Searching for key:", modelData.dateKey); 
                                
                                var status = calendarGrid.validDates[modelData.dateKey];
                                
                                if (status === undefined) {
                                    // This line runs because the key doesn't match
                                    console.log("CRITICAL MISMATCH! Searching for key: '" + modelData.dateKey + "'");
                                    console.log("Available keys in Map: " + Object.keys(calendarGrid.validDates).join(", "));
                                    return "gray"; 
                                }
                                
                                return status === true ? "#86efac" : "#fca5a5";
                            }
                        }

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

                        /**
                         * Exam details shown only when the day has at least one
                         * exam. Iterates modelData.exams so every exam scheduled
                         * on the day is rendered, one block per exam.
                         */
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            anchors.topMargin: 24
                            spacing: 8
                            visible: modelData.hasExam === true

                            Repeater {
                                model: modelData.exams

                                /**
                                 * One exam entry: a padded text block followed by
                                 * a thin separator drawn between consecutive exams
                                 * only (never after the last one). Keeps the
                                 * original colors, fonts and right alignment.
                                 */
                                delegate: ColumnLayout {
                                    id: examCard
                                    Layout.fillWidth: true
                                    spacing: 6

                                    /**
                                     * Drag bookkeeping.
                                     *
                                     * Stores the cell-side parent and the on-grid
                                     * return position/size captured at drag start,
                                     * so the card can bounce back and re-attach to
                                     * its origin cell on drop.
                                     */
                                    property int capturedCourseId: (typeof modelData !== 'undefined' && modelData !== null) ? modelData.courseId : -1
                                    property var originalParent: null
                                    property real returnX: 0
                                    property real returnY: 0
                                    property real startWidth: 0
                                    property real startHeight: 0

                                    /**
                                     * Makes the exam card draggable. On start the
                                     * card detaches from its cell and floats on
                                     * dragLayer at the same screen position/size;
                                     * on drop it resolves the target cell, runs the
                                     * mock move request, then bounces home.
                                     */
                                    DragHandler {
                                        target: examCard

                                        onActiveChanged: {
                                            if (active) {
                                                /** START: capture origin, reparent to the floating layer. */
                                                var pos = examCard.mapToItem(dragLayer, 0, 0)
                                                examCard.originalParent = examCard.parent
                                                examCard.startWidth = examCard.width
                                                examCard.startHeight = examCard.height
                                                examCard.returnX = pos.x
                                                examCard.returnY = pos.y

                                                examCard.parent = dragLayer
                                                examCard.x = pos.x
                                                examCard.y = pos.y
                                                examCard.width = examCard.startWidth
                                                examCard.height = examCard.startHeight

                                                calendarGrid.draggedCourseId = capturedCourseId
                                                calendarGrid.validDates =
                                                    mockScheduleManager.getValidDatesForCourse(capturedCourseId)
                                                calendarGrid.dragActive = true
                                            } else {
                                                /** DROP: resolve target cell at the card center, request the move. */
                                                calendarGrid.dragActive = false

                                                var center = examCard.mapToItem(
                                                    calendarGrid.contentItem,
                                                    examCard.width / 2, examCard.height / 2)
                                                var idx = calendarGrid.indexAt(center.x, center.y)

                                                if (idx >= 0 && capturedCourseId !== -1) {
                                                    var cell = appController.outputManager.currentCalendarData[idx]
                                                    var dk = cell ? cell.dateKey : ""
                                                    if (dk && dk !== "") {
                                                        var result = mockScheduleManager.requestMove(capturedCourseId, dk);
                                                        console.log("DROP", capturedCourseId, "→", dk, "| status =", result.status);
                                                    }
                                                }

                                                /** For now every drop bounces back to the origin cell. */
                                                bounceBack.start()
                                            }
                                        }
                                    }

                                    HoverHandler {
                                        cursorShape: Qt.PointingHandCursor
                                    }

                                    /**
                                     * Return-to-origin animation. Eases the card
                                     * back to its captured position with an OutBack
                                     * bounce, then re-attaches it to its cell.
                                     */
                                    SequentialAnimation {
                                        id: bounceBack

                                        ParallelAnimation {
                                            NumberAnimation {
                                                target: examCard; property: "x"
                                                to: examCard.returnX
                                                duration: 220; easing.type: Easing.OutBack
                                            }
                                            NumberAnimation {
                                                target: examCard; property: "y"
                                                to: examCard.returnY
                                                duration: 220; easing.type: Easing.OutBack
                                            }
                                        }

                                        ScriptAction {
                                            script: {
                                                examCard.parent = examCard.originalParent
                                                examCard.x = 0
                                                examCard.y = 0
                                            }
                                        }
                                    }

                                    /**
                                     * Padded exam block so each exam breathes
                                     * instead of touching the cell edges and the
                                     * separator.
                                     */
                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        Layout.leftMargin: 4
                                        Layout.rightMargin: 4
                                        Layout.topMargin: 4
                                        Layout.bottomMargin: 4
                                        spacing: 2

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
                                            text: "👤 " + (modelData.program || "")
                                            font.pixelSize: 10
                                            color: "#14533f"
                                            elide: Text.ElideRight
                                            horizontalAlignment: Text.AlignRight
                                        }
                                    }

                                    /**
                                     * Thin separator between two consecutive
                                     * exams. Hidden after the last exam so no
                                     * trailing line is drawn.
                                     */
                                    Rectangle {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 1
                                        color: "#e2e8f0"
                                        visible: index < (dayCell.examCount - 1)
                                    }
                                }
                            }
                        }
                    }
                }

                /**
                 * Floating layer hosting a dragged exam card while it travels.
                 *
                 * A card is reparented here on drag start so it can move freely
                 * above the grid and its clipping, and reparented back on drop.
                 */
                Item {
                    id: dragLayer
                    anchors.fill: calendarGrid
                    z: 1000
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
}
