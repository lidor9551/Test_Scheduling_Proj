import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

/**
 * SettingsScreen.qml
 *
 * Hard constraint configuration panel for the exam scheduler.
 * Each of the five rules exposes an on/off Switch and an integer threshold k.
 * State is stored in local properties; AppController wiring is done in subtask B.
 */
Item {
    id: settingsRoot

    // -------------------------------------------------------------------------
    // Local color palette — Main.qml root is out of scope from a pushed screen.
    // -------------------------------------------------------------------------
    readonly property color primary:     "#14533f"
    readonly property color primaryDark: "#0f3f30"
    readonly property color borderSoft:  "#e1e5df"
    readonly property color textDark:    "#1f2933"
    readonly property color textMuted:   "#69737a"
    readonly property color pageBg:      "#f7f5ef"

    // -------------------------------------------------------------------------
    // Public constraint properties — five on/off flags and five k thresholds.
    // These will be read by AppController in subtask B.
    // -------------------------------------------------------------------------

    /** Rule 2.1: minimum gap between mandatory exams (same program, year). */
    property bool rule21Enabled: false
    property int  rule21K:       1

    /** Rule 2.2: minimum gap between any two exams (same program, year). */
    property bool rule22Enabled: false
    property int  rule22K:       1

    /** Rule 2.3: maximum conflicts between optional courses (same program). */
    property bool rule23Enabled: false
    property int  rule23K:       1

    /** Rule 2.4: spread of mandatory exams (same program, year, moed). */
    property bool rule24Enabled: false
    property int  rule24K:       1

    /** Rule 2.5: maximum exams scheduled on the same day. */
    property bool rule25Enabled: false
    property int  rule25K:       1

    // -------------------------------------------------------------------------
    // Declarative rule model — drives the Repeater.
    // enabledProp / kProp are string keys used for dynamic property access on
    // settingsRoot, avoiding five near-identical delegate blocks.
    // -------------------------------------------------------------------------
    readonly property var ruleModel: [
        {
            tag:         "2.1",
            title:       "Minimum gap between mandatory exams",
            description: "Minimum days between two mandatory exams in the same program and year must be >= k",
            enabledProp: "rule21Enabled",
            kProp:       "rule21K"
        },
        {
            tag:         "2.2",
            title:       "Minimum gap between any two exams",
            description: "Minimum days between any two exams in the same program and year must be >= k",
            enabledProp: "rule22Enabled",
            kProp:       "rule22K"
        },
        {
            tag:         "2.3",
            title:       "Maximum optional course conflicts",
            description: "Maximum conflicts between two optional courses in the same program must be <= k",
            enabledProp: "rule23Enabled",
            kProp:       "rule23K"
        },
        {
            tag:         "2.4",
            title:       "Mandatory exam spread",
            description: "Gap between the first and last mandatory exam (same program, year, moed) must be >= k",
            enabledProp: "rule24Enabled",
            kProp:       "rule24K"
        },
        {
            tag:         "2.5",
            title:       "Maximum exams per day",
            description: "Maximum number of exams scheduled on the same day must be <= k",
            enabledProp: "rule25Enabled",
            kProp:       "rule25K"
        }
    ]

    /**
     * Restores constraint state from AppController when the screen opens.
     * The StackView creates a fresh instance on each push, so values must be
     * explicitly re-read from C++ to reflect whatever was last saved.
     */
    Component.onCompleted: {
        var data = appController.getHardConstraints()
        settingsRoot.rule21Enabled = data.rule21Enabled
        settingsRoot.rule21K       = data.rule21K
        settingsRoot.rule22Enabled = data.rule22Enabled
        settingsRoot.rule22K       = data.rule22K
        settingsRoot.rule23Enabled = data.rule23Enabled
        settingsRoot.rule23K       = data.rule23K
        settingsRoot.rule24Enabled = data.rule24Enabled
        settingsRoot.rule24K       = data.rule24K
        settingsRoot.rule25Enabled = data.rule25Enabled
        settingsRoot.rule25K       = data.rule25K
    }

    // Page background fill.
    Rectangle {
        anchors.fill: parent
        color: settingsRoot.pageBg
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 34
        spacing: 22

        // ----- Header --------------------------------------------------------
        RowLayout {
            Layout.fillWidth: true
            spacing: 16

            /** Back button — pops this screen off the StackView without saving. */
            Button {
                text: "← Back"
                font.pixelSize: 16
                font.bold: true
                background: Rectangle {
                    color: parent.down    ? "#e2e8f0"
                         : parent.hovered ? "#f1f5f9"
                         : "transparent"
                    radius: 8
                }
                onClicked: {
                    if (settingsRoot.StackView.view) {
                        settingsRoot.StackView.view.pop()
                    }
                }
            }

            /**
             * Save button — forwards all ten constraint values to AppController
             * via saveHardConstraints(), then returns to the input screen.
             */
            Button {
                text: "Save"
                font.pixelSize: 16
                font.bold: true
                implicitWidth:  100
                implicitHeight: 46
                background: Rectangle {
                    color: parent.down    ? settingsRoot.primaryDark
                         : parent.hovered ? "#1b664f"
                         : settingsRoot.primary
                    radius: 8
                }
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.pixelSize: 16
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment:   Text.AlignVCenter
                }
                onClicked: {
                    appController.saveHardConstraints(
                        settingsRoot.rule21Enabled, settingsRoot.rule21K,
                        settingsRoot.rule22Enabled, settingsRoot.rule22K,
                        settingsRoot.rule23Enabled, settingsRoot.rule23K,
                        settingsRoot.rule24Enabled, settingsRoot.rule24K,
                        settingsRoot.rule25Enabled, settingsRoot.rule25K
                    )
                    if (settingsRoot.StackView.view) {
                        settingsRoot.StackView.view.pop()
                    }
                }
            }

            Item { Layout.fillWidth: true }

            Text {
                text: "Settings"
                font.pixelSize: 36
                font.bold: true
                color: settingsRoot.textDark
            }
        }

        // ----- Subtitle ------------------------------------------------------
        Text {
            Layout.fillWidth: true
            text: "Enable or disable hard scheduling constraints and set their threshold value k."
            color: settingsRoot.textMuted
            font.pixelSize: 14
            wrapMode: Text.WordWrap
        }

        // ----- Constraint cards ----------------------------------------------
        Repeater {
            model: settingsRoot.ruleModel

            /**
             * One card per constraint rule.
             * Card height is driven by the inner RowLayout's implicit height so
             * that wrapped description text never clips.
             */
            delegate: Rectangle {
                Layout.fillWidth: true
                implicitHeight: cardRow.implicitHeight + 40

                radius: 16
                color: "white"
                border.color: settingsRoot.borderSoft
                border.width: 1

                RowLayout {
                    id: cardRow
                    anchors {
                        left:        parent.left
                        right:       parent.right
                        top:         parent.top
                        leftMargin:  20
                        rightMargin: 20
                        topMargin:   20
                    }
                    spacing: 16

                    /** Toggle that enables or disables this constraint. */
                    Switch {
                        id: ruleSwitch
                        // One-time initialisation from the backing property.
                        // onToggled (user-only signal) writes the change back
                        // without triggering a binding loop.
                        checked: settingsRoot[modelData.enabledProp]
                        onToggled: settingsRoot[modelData.enabledProp] = checked
                    }

                    // Rule number badge.
                    Rectangle {
                        width: 40
                        height: 40
                        radius: 10
                        color: ruleSwitch.checked ? "#edf7f2" : "#f1f5f9"

                        Text {
                            anchors.centerIn: parent
                            text: modelData.tag
                            font.pixelSize: 12
                            font.bold: true
                            color: ruleSwitch.checked ? settingsRoot.primary
                                                      : settingsRoot.textMuted
                        }
                    }

                    // Rule title and description.
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 4

                        Text {
                            Layout.fillWidth: true
                            text: modelData.title
                            font.pixelSize: 15
                            font.bold: true
                            color: ruleSwitch.checked ? settingsRoot.textDark
                                                      : settingsRoot.textMuted
                            wrapMode: Text.WordWrap
                        }

                        Text {
                            Layout.fillWidth: true
                            text: modelData.description
                            font.pixelSize: 13
                            color: settingsRoot.textMuted
                            wrapMode: Text.WordWrap
                        }
                    }

                    // k threshold label and spinner.
                    ColumnLayout {
                        spacing: 4

                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            text: "k"
                            font.pixelSize: 13
                            font.bold: true
                            color: ruleSwitch.checked ? settingsRoot.textDark
                                                      : settingsRoot.textMuted
                        }

                        /**
                         * Integer threshold input.
                         * Disabled when the rule toggle is off.
                         * onValueChanged fires on every change — buttons, keyboard, and
                         * programmatic updates from Component.onCompleted. No binding loop
                         * risk: QML skips the property-change notification when the written
                         * value is identical to the current one.
                         */
                        SpinBox {
                            from:    0
                            to:      99
                            value:   settingsRoot[modelData.kProp]
                            enabled: ruleSwitch.checked
                            onValueChanged: settingsRoot[modelData.kProp] = value
                        }
                    }
                }
            }
        }

        // Pushes the cards toward the top when the window is taller than the content.
        Item { Layout.fillHeight: true }
    }
}
