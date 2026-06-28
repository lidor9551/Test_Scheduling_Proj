import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

/**
 * InputScreen.qml — data input & study-program selection (the app's home screen).
 *
 * Extracted verbatim from the former inline `inputPage` in Main.qml; it is the
 * StackView's initialItem. Dependencies that used to live in Main's root scope
 * moved here because they are only used by this screen:
 *   - the "view courses" Dialog (courseDetailsDialog) and the program it shows
 *     (currentProgramId / currentProgramName),
 *   - the FileCard component.
 * Navigation to the other screens goes through this page's own StackView.view,
 * so Main keeps only the window, the StackView and global wiring.
 *
 * Colors and the spacing/radius scale come from AppTheme — no hardcoded hex.
 */
Item {
    id: inputScreen

    AppTheme { id: theme }

    // Study program currently shown in the "view courses" dialog.
    property string currentProgramId: ""
    property string currentProgramName: ""

    // ── "View courses" dialog (used only by this screen) ─────────────────────
    Dialog {
        id: courseDetailsDialog
        title: "קורסים לתוכנית: " + inputScreen.currentProgramName + " (" + inputScreen.currentProgramId + ")"
        width: 600; height: 500
        anchors.centerIn: parent
        modal: true
        standardButtons: Dialog.Close

        function refreshData() {
            // year 1 is index 1, year 2 is index 2, etc. 'הכל' is index 0, so we return -1 for that case to indicate no filtering by year.
            let year = yearCombo.currentIndex === 0 ? -1 : yearCombo.currentIndex;

            // semester: 'הכל' is index 0, 'א' is index 1, 'ב' is index 2, 'קיץ' is index 3.
            // in C++ your Enum starts from 0, so we subtract 1. (0 becomes -1, meaning all).
            let sem = semesterCombo.currentIndex === 0 ? -1 : semesterCombo.currentIndex - 1;

            courseListModel.model = appController.getCoursesForProgram(inputScreen.currentProgramId, year, sem);
        }

        // reset filters and refresh data every time dialog is opened
        onOpened: {
            yearCombo.currentIndex = 0;
            semesterCombo.currentIndex = 0;
            refreshData();
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            // filter section (with right-to-left layout)
            Rectangle {
                Layout.fillWidth: true
                height: 60
                color: "white"

                Row {
                    anchors.centerIn: parent
                    spacing: 15
                    layoutDirection: Qt.RightToLeft

                    Text { text: "שנה:"; anchors.verticalCenter: parent.verticalCenter; font.bold: true }
                    ComboBox {
                        id: yearCombo
                        width: 100
                        model: ["הכל", "שנה א'", "שנה ב'", "שנה ג'", "שנה ד'"]
                        onActivated: courseDetailsDialog.refreshData()
                    }

                    Text { text: "סמסטר:"; anchors.verticalCenter: parent.verticalCenter; font.bold: true }
                    ComboBox {
                        id: semesterCombo
                        width: 120
                        model: ["הכל", "סמסטר א'", "סמסטר ב'", "סמסטר קיץ"]
                        onActivated: courseDetailsDialog.refreshData()
                    }
                }
            }

            // header row for the course list
            Rectangle {
                Layout.fillWidth: true
                height: 40
                color: theme.neutralBg
                Row {
                    anchors.fill: parent
                    padding: 10
                    spacing: 20
                    Text { text: "שם הקורס"; font.bold: true; width: 300; horizontalAlignment: Text.AlignLeft }
                    Text { text: "דרישה"; font.bold: true; width: 80; horizontalAlignment: Text.AlignRight }
                    Text { text: "הערכה"; font.bold: true; width: 100; horizontalAlignment: Text.AlignRight }
                }
            }

            // the scrollable list of courses
            ListView {
                id: courseListModel
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true

                delegate: Rectangle {
                    width: parent.width; height: 45
                    color: index % 2 === 0 ? "white" : theme.neutralBgAlt
                    border.color: theme.neutralBorder; border.width: 1

                    Row {
                        anchors.fill: parent; padding: 10; spacing: 20

                        Text { text: modelData.name !== undefined ? modelData.name : ""; width: 300; elide: Text.ElideRight; horizontalAlignment: Text.AlignLeft }
                        Text { text: modelData.req !== undefined ? modelData.req : ""; color: text === "חובה" ? theme.cellExcludedText : theme.successText; width: 80; horizontalAlignment: Text.AlignRight }
                        Text { text: modelData.eval !== undefined ? modelData.eval : ""; color: theme.textSteel; width: 100; horizontalAlignment: Text.AlignRight }
                    }
                }
            }
        }
    }

    // ── File selection card (used only by this screen) ───────────────────────
    component FileCard: Rectangle {
        id: card

        property string titleText: ""
        property string descriptionText: ""
        property string iconText: ""
        property string selectedPath: ""

        property string emptyTitle: "בחר קובץ"
        property string emptyDesc: "לחץ כאן לבחירת קובץ txt"
        property string filledTitle: "קובץ נבחר בהצלחה"

        signal browseClicked()

        Layout.fillWidth: true
        Layout.preferredHeight: 215

        radius: theme.radiusXL
        color: "white"
        border.color: selectedPath.length > 0 ? theme.primary : theme.borderSoft
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
                    color: theme.primarySoft

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
                        color: theme.textDark
                        font.pixelSize: 23
                        font.bold: true
                        horizontalAlignment: Text.AlignRight
                    }

                    Text {
                        Layout.fillWidth: true
                        text: card.descriptionText
                        color: theme.textMuted
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
                color: selectedPath.length > 0 ? theme.cardFilledBg : theme.cardEmptyBg
                border.color: selectedPath.length > 0 ? theme.cardFilledBorder : theme.cardEmptyBorder
                border.width: 1

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: card.browseClicked()
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: theme.spacingL
                    spacing: 12

                    Rectangle {
                        width: 42
                        height: 42
                        radius: theme.radius
                        color: selectedPath.length > 0 ? theme.checkIconBg : theme.uploadIconBg

                        Text {
                            anchors.centerIn: parent
                            text: selectedPath.length > 0 ? "✓" : "↑"
                            color: selectedPath.length > 0 ? theme.success : theme.primary
                            font.pixelSize: 24
                            font.bold: true
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 3

                        Text {
                            Layout.fillWidth: true
                            text: selectedPath.length > 0 ? card.filledTitle : card.emptyTitle
                            color: theme.textDark
                            font.pixelSize: 16
                            font.bold: true
                            horizontalAlignment: Text.AlignRight
                        }

                        Text {
                            Layout.fillWidth: true
                            LayoutMirroring.enabled: false
                            text: selectedPath.length > 0 ? selectedPath : card.emptyDesc
                            color: theme.textMuted
                            font.pixelSize: 12
                            elide: Text.ElideMiddle
                            horizontalAlignment: selectedPath.length > 0 ? Text.AlignLeft : Text.AlignRight
                        }
                    }
                }
            }
        }
    }

    // Windows/Mac File Dialog for Courses
    FileDialog {
        id: coursesDialog
        title: "בחר קובץ קורסים"
        nameFilters: ["קבצי טקסט (*.txt)", "כל הקבצים (*)"]
        onAccepted: appController.setCoursesFilePath(selectedFile)
    }

    // Windows/Mac File Dialog for Periods
    FileDialog {
        id: periodsDialog
        title: "בחר קובץ תאריכים"
        nameFilters: ["קבצי טקסט (*.txt)", "כל הקבצים (*)"]
        onAccepted: appController.setExamPeriodsFilePath(selectedFile)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: theme.marginL
        spacing: theme.spacingXL

        // Scrollable content area: header + file cards + program list.
        // Separated from the action bar (kept as a fixed footer below) so
        // the action buttons can never be pushed off a small window.
        ScrollView {
            id: contentScroll
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            contentWidth: availableWidth   // content never scrolls sideways

            ColumnLayout {
                id: scrollContent
                width: contentScroll.availableWidth
                // Match the viewport on large windows (identical layout),
                // but become taller than it on small windows so the
                // vertical scroll bar appears instead of clipping content.
                height: Math.max(implicitHeight, contentScroll.availableHeight)
                spacing: theme.spacingXL

                // Header Section
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 6

                    Text {
                        Layout.fillWidth: true
                        text: "דף הבית / קלט נתונים"
                        color: theme.textBreadcrumb
                        font.pixelSize: 13
                        horizontalAlignment: Text.AlignRight
                    }

                    Text {
                        Layout.fillWidth: true
                        text: "קלט נתונים"
                        color: theme.textDark
                        font.pixelSize: 36
                        font.bold: true
                        horizontalAlignment: Text.AlignRight
                    }

                    Text {
                        Layout.fillWidth: true
                        text: "טען קבצי קורסים ותאריכים, ולאחר מכן בחר האם להחליף את הנתונים הקיימים או להוסיף אותם למידע הקיים."
                        color: theme.textMuted
                        font.pixelSize: 16
                        horizontalAlignment: Text.AlignRight
                        wrapMode: Text.WordWrap
                    }
                }

                // Top Cards Section
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

                // Bottom Section: Program Selection List
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true // Takes all remaining vertical space
                    Layout.minimumHeight: 360 // keep the list usable once the page scrolls
                    radius: theme.radiusXL
                    color: "white"
                    border.color: appController.selectedPrograms.length > 0 ? theme.primary : theme.borderSoft
                    border.width: appController.selectedPrograms.length > 0 ? 1.5 : 1
                    clip: true

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 24
                        spacing: theme.spacingL

                        // Section Header
                        RowLayout {
                            Layout.fillWidth: true
                            layoutDirection: Qt.RightToLeft
                            spacing: theme.spacingL

                            Rectangle {
                                width: 46
                                height: 46
                                radius: 14
                                color: theme.primarySoft
                                Text {
                                    anchors.centerIn: parent
                                    text: "📋"
                                    font.pixelSize: 22
                                }
                            }

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 2
                                Text {
                                    text: "בחירת תוכניות לימוד"
                                    font.pixelSize: 22
                                    font.bold: true
                                    color: theme.textDark
                                    horizontalAlignment: Text.AlignRight
                                    Layout.fillWidth: true
                                }
                                Text {
                                    text: appController.selectedPrograms.length + " / 5 תוכניות נבחרו"
                                    font.pixelSize: 14
                                    color: appController.selectedPrograms.length > 0 ? theme.success : theme.textMuted
                                    horizontalAlignment: Text.AlignRight
                                    Layout.fillWidth: true
                                }
                            }
                        }

                        // Divider line
                        Rectangle {
                            Layout.fillWidth: true
                            height: 1
                            color: theme.borderSoft
                        }

                        // Local Data Model holding the exact programs provided
                        ListModel {
                            id: programsModel
                            ListElement { progId: "83101"; progName: "הנדסת מחשבים" }
                            ListElement { progId: "83102"; progName: "הנדסת חשמל" }
                            ListElement { progId: "83104"; progName: "הנדסת תעשיה ומערכות מידע" }
                            ListElement { progId: "83107"; progName: "הנדסת נתונים" }
                            ListElement { progId: "83108"; progName: "הנדסת תוכנה" }
                            ListElement { progId: "83109"; progName: "הנדסת חומרים" }
                            ListElement { progId: "83105"; progName: "הנדסת מחשבים – מגמת חומרת מחשבים" }
                            ListElement { progId: "83182"; progName: "הנדסת חשמל – מגמת הנדסה קוונטית" }
                            ListElement { progId: "83103"; progName: "הנדסת חשמל – מגמת נוירו הנדסה" }
                            ListElement { progId: "83115"; progName: "הנדסת חשמל – מגמת הנדסה ביו-רפואית" }
                        }

                        // The Scrollable List of Programs
                        ListView {
                            id: programListView
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            clip: true
                            spacing: theme.spacingM
                            model: programsModel

                            ScrollBar.vertical: ScrollBar {
                                active: true
                            }

                            // The styled rectangle for each program
                            delegate: Rectangle {
                                width: programListView.width - 20
                                height: 72
                                radius: theme.radius

                                property bool isChecked: appController.selectedPrograms.indexOf(progId) !== -1
                                property bool canBeChecked: isChecked || appController.selectedPrograms.length < 5

                                color: isChecked ? theme.selectedBg : (itemMouseArea.containsMouse ? theme.neutralBgAlt : "white")
                                border.color: isChecked ? theme.success : theme.borderSoft
                                border.width: isChecked ? 2 : 1

                                // Makes the entire row clickable
                                MouseArea {
                                    id: itemMouseArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    cursorShape: parent.canBeChecked ? Qt.PointingHandCursor : Qt.ArrowCursor
                                    onClicked: {
                                        if (parent.canBeChecked) {
                                            appController.toggleProgram(progId)
                                        }
                                    }
                                }

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: theme.spacingL
                                    layoutDirection: Qt.RightToLeft

                                    CheckBox {
                                        checked: parent.parent.isChecked
                                        enabled: parent.parent.canBeChecked
                                        // Disable hover on checkbox itself so it doesn't conflict with MouseArea
                                        hoverEnabled: false
                                        onClicked: {
                                            appController.toggleProgram(progId)
                                        }
                                    }

                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 4

                                        Text {
                                            text: progName
                                            font.pixelSize: 16
                                            font.bold: true
                                            color: theme.textDark
                                            horizontalAlignment: Text.AlignRight
                                            Layout.fillWidth: true
                                        }

                                        Text {
                                            text: "מספר תוכנית: " + progId
                                            font.pixelSize: 13
                                            color: theme.textMuted
                                            horizontalAlignment: Text.AlignRight
                                            Layout.fillWidth: true
                                        }
                                    }

                                    Item { Layout.fillWidth: true } // Spacer

                                    // Eye button for course details
                                    Button {
                                        text: "👁️ צפה בקורסים"
                                        font.pixelSize: 13
                                        background: Rectangle {
                                            color: parent.down ? theme.neutralBorder : (parent.hovered ? theme.neutralBg : "transparent")
                                            border.color: theme.borderSoft
                                            border.width: 1
                                            radius: theme.radiusS
                                        }
                                        onClicked: {
                                            inputScreen.currentProgramId = progId;
                                            inputScreen.currentProgramName = progName;
                                            courseDetailsDialog.open();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            } // scrollContent
        } // contentScroll

        // Action Buttons and System Messages Section — fixed footer.
        // Always visible regardless of window size; its row scrolls
        // horizontally so no button is ever clipped sideways.
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 92
            radius: theme.radiusL
            color: appController.errorMessage.length > 0
                   ? theme.softRedBg
                   : appController.statusMessage.length > 0
                     ? theme.statusOkBg
                     : "white"
            border.color: appController.errorMessage.length > 0
                          ? theme.softRedBorder
                          : appController.statusMessage.length > 0
                            ? theme.statusOkBorder
                            : theme.borderSoft
            border.width: 1

            ScrollView {
                id: footerScroll
                anchors.fill: parent
                anchors.margins: 20
                clip: true
                // Horizontal scrolling only; the footer height is fixed.
                contentHeight: availableHeight
                ScrollBar.vertical.policy: ScrollBar.AlwaysOff
                ScrollBar.horizontal.policy: ScrollBar.AsNeeded

                RowLayout {
                    // Equal to the viewport on wide windows (the status text
                    // sits at the far edge exactly as before); expands to its
                    // implicit width on narrow ones to enable scrolling.
                    width: Math.max(implicitWidth, footerScroll.availableWidth)
                    height: footerScroll.availableHeight
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
                        text: "📅 צפה בלוח השנה"
                        outline: false
                        visible: appController.hasData
                        onClicked: {
                            calendarManager.setData(
                                appController.examPeriods,
                                appController.courses
                            )
                            inputScreen.StackView.view.push(Qt.resolvedUrl("CalendarScreen.qml"))
                        }
                    }
                    AppButton {
                        text: "צור מערכות שיבוץ"
                        outline: false
                        // check if there's at least one program selected before showing the button
                        visible: appController.hasData && appController.selectedPrograms.length > 0
                        onClicked: {
                            appController.generateSchedules()

                            // go to the output screen after generating schedules
                            inputScreen.StackView.view.push(Qt.resolvedUrl("OutputScreen.qml"))
                        }
                    }

                    /**
                     * Settings button — opens the hard constraint configuration screen.
                     * Gated on hasData like the Calendar and Generate buttons: the k upper
                     * bound (kMax) is derived from the loaded exam periods, so opening this
                     * screen before any data is loaded would only show the fallback bound.
                     */
                    AppButton {
                        text: "הגדרות"
                        outline: true
                        visible: appController.hasData
                        onClicked: inputScreen.StackView.view.push(Qt.resolvedUrl("SettingsScreen.qml"))
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
                               ? theme.danger
                               : appController.statusMessage.length > 0
                                 ? theme.success
                                 : theme.textMuted
                        font.pixelSize: 14
                        font.bold: appController.statusMessage.length > 0 || appController.errorMessage.length > 0
                        horizontalAlignment: Text.AlignRight
                        wrapMode: Text.WordWrap
                    }
                }
            }
        }
    }
}
