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
    title: "מתזמן בחינות 2.0 - קלט"
    color: theme.pageBg

    AppTheme { id: theme }

    property color primary: theme.primary
    property color primaryDark: theme.primaryDark
    property color primarySoft: theme.primarySoft
    property color borderSoft: theme.borderSoft
    property color textDark: theme.textDark
    property color textMuted: theme.textMuted
    property color danger: theme.danger
    property color success: theme.success

    property string currentProgramView: ""
    property string currentProgramId: ""  
    property string currentProgramName: ""
    property var mainStackView: stackView
    
    
    Dialog {
    id: courseDetailsDialog
    title: "קורסים לתוכנית: " + root.currentProgramName + " (" + root.currentProgramId + ")"
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
        
        courseListModel.model = appController.getCoursesForProgram(root.currentProgramId, year, sem);
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
            color: "#f1f5f9"
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
                color: index % 2 === 0 ? "#ffffff" : "#f8fafc"
                border.color: "#e2e8f0"; border.width: 1
                
                Row {
                    anchors.fill: parent; padding: 10; spacing: 20
                    
                    Text { text: modelData.name !== undefined ? modelData.name : ""; width: 300; elide: Text.ElideRight; horizontalAlignment: Text.AlignLeft }
                    Text { text: modelData.req !== undefined ? modelData.req : ""; color: text === "חובה" ? "#dc2626" : "#059669"; width: 80; horizontalAlignment: Text.AlignRight }
                    Text { text: modelData.eval !== undefined ? modelData.eval : ""; color: "#475569"; width: 100; horizontalAlignment: Text.AlignRight }
                }
            }
        }
    }
}
    // Custom Card Component for file selection
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
                            text: selectedPath.length > 0 ? card.filledTitle : card.emptyTitle
                            color: root.textDark
                            font.pixelSize: 16
                            font.bold: true
                            horizontalAlignment: Text.AlignRight
                        }

                        Text {
                            Layout.fillWidth: true
                            LayoutMirroring.enabled: false
                            text: selectedPath.length > 0 ? selectedPath : card.emptyDesc
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

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: inputPage
    }

    Component {
        id: inputPage

        Item {

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
                anchors.margins: 34
                spacing: 22

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
                        spacing: 22

                // Header Section
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
                    radius: 18
                    color: "white"
                    border.color: appController.selectedPrograms.length > 0 ? root.primary : root.borderSoft
                    border.width: appController.selectedPrograms.length > 0 ? 1.5 : 1
                    clip: true

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 24
                        spacing: 16

                        // Section Header
                        RowLayout {
                            Layout.fillWidth: true
                            layoutDirection: Qt.RightToLeft
                            spacing: 16

                            Rectangle {
                                width: 46
                                height: 46
                                radius: 14
                                color: root.primarySoft
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
                                    color: root.textDark
                                    horizontalAlignment: Text.AlignRight
                                    Layout.fillWidth: true
                                }
                                Text {
                                    text: appController.selectedPrograms.length + " / 5 תוכניות נבחרו"
                                    font.pixelSize: 14
                                    color: appController.selectedPrograms.length > 0 ? root.success : root.textMuted
                                    horizontalAlignment: Text.AlignRight
                                    Layout.fillWidth: true
                                }
                            }
                        }

                        // Divider line
                        Rectangle {
                            Layout.fillWidth: true
                            height: 1
                            color: root.borderSoft
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
                            spacing: 12
                            model: programsModel
                            
                            ScrollBar.vertical: ScrollBar { 
                                active: true 
                            }

                            // The styled rectangle for each program
                            delegate: Rectangle {
                                width: programListView.width - 20
                                height: 72
                                radius: 12
                                
                                property bool isChecked: appController.selectedPrograms.indexOf(progId) !== -1
                                property bool canBeChecked: isChecked || appController.selectedPrograms.length < 5
                                
                                color: isChecked ? "#f0fdf4" : (itemMouseArea.containsMouse ? "#f8fafc" : "white")
                                border.color: isChecked ? root.success : root.borderSoft
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
                                    anchors.margins: 16
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
                                            color: root.textDark
                                            horizontalAlignment: Text.AlignRight
                                            Layout.fillWidth: true
                                        }
                                        
                                        Text {
                                            text: "מספר תוכנית: " + progId
                                            font.pixelSize: 13
                                            color: root.textMuted
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
                                            color: parent.down ? "#e2e8f0" : (parent.hovered ? "#f1f5f9" : "transparent")
                                            border.color: root.borderSoft
                                            border.width: 1
                                            radius: 8
                                        }
                                        onClicked: {
                                            root.currentProgramId = progId;
                                            root.currentProgramName = progName;
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
                                    stackView.push(Qt.resolvedUrl("CalendarScreen.qml"))
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
                                    stackView.push(Qt.resolvedUrl("OutputScreen.qml"))
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
                                onClicked: stackView.push(Qt.resolvedUrl("SettingsScreen.qml"))
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
                }
            }
        }
    }
}