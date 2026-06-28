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
    AppTheme { id: theme }

    readonly property color primary:     theme.primary
    readonly property color primaryDark: theme.primaryDark
    readonly property color borderSoft:  theme.borderSoft
    readonly property color textDark:    theme.textDark
    readonly property color textMuted:   theme.textMuted
    readonly property color pageBg:      theme.pageBg

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

    /**
     * Upper bound applied to every k SpinBox.
     * Derived from the exam period length in Component.onCompleted; a value of k
     * larger than the whole exam window is meaningless. Falls back to 31 days
     * when no exam periods are loaded yet.
     */
    property int kMax: 31

    // -------------------------------------------------------------------------
    // Global overflow state — aggregated from the per-rule delegates.
    // -------------------------------------------------------------------------

    /**
     * Per-rule overflow flags, one entry per rule in ruleModel.
     * Each delegate reports its own state into this array through
     * setRuleOverflow(); hasOverflow then reflects "at least one true".
     * Kept as a plain array updated imperatively to avoid any binding loop.
     */
    property var ruleOverflow: [false, false, false, false, false]

    /**
     * True as soon as at least one rule holds a k value greater than kMax.
     * Drives the Save button's enabled state and its grayed-out styling.
     * Written only from setRuleOverflow() (never bound), so no binding loop.
     */
    property bool hasOverflow: false

    /**
     * Visibility of the temporary global overflow message.
     * Raised on each out-of-range keystroke and auto-cleared after 5 seconds by
     * overflowMessageTimer. Intentionally separate from hasOverflow: the message
     * is a short reminder, while Save stays disabled for as long as the value is
     * actually out of range.
     */
    property bool overflowMessageVisible: false

    /**
     * Records the overflow state of a single rule and refreshes hasOverflow.
     *
     * @param idx   Index of the rule in ruleModel (the Repeater index).
     * @param state True when the rule's typed k currently exceeds kMax.
     */
    function setRuleOverflow(idx, state) {
        var arr = settingsRoot.ruleOverflow
        arr[idx] = state
        settingsRoot.ruleOverflow = arr

        var any = false
        for (var i = 0; i < arr.length; ++i) {
            if (arr[i]) {
                any = true
                break
            }
        }
        settingsRoot.hasOverflow = any
    }

    /**
     * Shows the temporary global overflow message and (re)starts its 5s timer.
     * Calling it again before the timer fires restarts the countdown.
     */
    function flashOverflowMessage() {
        settingsRoot.overflowMessageVisible = true
        overflowMessageTimer.restart()
    }

    /** Auto-dismisses the global overflow message 5 seconds after the last attempt. */
    Timer {
        id: overflowMessageTimer
        interval: 5000
        onTriggered: settingsRoot.overflowMessageVisible = false
    }

    // -------------------------------------------------------------------------
    // Declarative rule model — drives the Repeater.
    // enabledProp / kProp are string keys used for dynamic property access on
    // settingsRoot, avoiding five near-identical delegate blocks.
    // -------------------------------------------------------------------------
    readonly property var ruleModel: [
        {
            tag:         "2.1",
            title:       "מרווח מינימלי בין בחינות חובה",
            description: "מספר הימים המינימלי בין שתי בחינות חובה באותה תוכנית ושנה חייב להיות >= k",
            enabledProp: "rule21Enabled",
            kProp:       "rule21K"
        },
        {
            tag:         "2.2",
            title:       "מרווח מינימלי בין שתי בחינות כלשהן",
            description: "מספר הימים המינימלי בין שתי בחינות כלשהן באותה תוכנית ושנה חייב להיות >= k",
            enabledProp: "rule22Enabled",
            kProp:       "rule22K"
        },
        {
            tag:         "2.3",
            title:       "מקסימום התנגשויות בין קורסי בחירה",
            description: "מספר ההתנגשויות המקסימלי בין שני קורסי בחירה באותה תוכנית חייב להיות <= k",
            enabledProp: "rule23Enabled",
            kProp:       "rule23K"
        },
        {
            tag:         "2.4",
            title:       "פריסת בחינות חובה",
            description: "המרווח בין בחינת החובה הראשונה לאחרונה (אותה תוכנית, שנה ומועד) חייב להיות >= k",
            enabledProp: "rule24Enabled",
            kProp:       "rule24K"
        },
        {
            tag:         "2.5",
            title:       "מקסימום בחינות ביום",
            description: "מספר הבחינות המקסימלי המשובצות באותו יום חייב להיות <= k",
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
        /**
         * Establish the real upper bound BEFORE restoring any saved k.
         *
         * Each k SpinBox binds "to: kMax" and writes its (already clamped) value
         * back into the rule property through onValueChanged, which also fires on
         * programmatic changes. If kMax were still its fallback (31) when a saved
         * value is assigned, the SpinBox would clamp a perfectly valid value such
         * as 33 down to 31 and push that wrong value back into the rule property,
         * permanently losing the saved value. Computing kMax first makes the
         * SpinBox range correct before any value is restored.
         */
        var days = appController.getExamPeriodDays()
        settingsRoot.kMax = (days > 0) ? days : 31

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

        /**
         * Silently clamp any stored k that genuinely exceeds the real kMax.
         * The exam period may have shrunk since these values were saved (e.g. the
         * user reduced a 31-day period to 20 days), making a previously valid k
         * out of range. Because kMax is now the true bound, this only fires for
         * values that are actually too large — never for a valid value such as 33
         * while kMax still held its fallback.
         * This is intentionally silent: no per-rule error is shown, since the user
         * did not just type the offending value — see kOverflow handling below.
         */
        if (settingsRoot.rule21K > settingsRoot.kMax) settingsRoot.rule21K = settingsRoot.kMax
        if (settingsRoot.rule22K > settingsRoot.kMax) settingsRoot.rule22K = settingsRoot.kMax
        if (settingsRoot.rule23K > settingsRoot.kMax) settingsRoot.rule23K = settingsRoot.kMax
        if (settingsRoot.rule24K > settingsRoot.kMax) settingsRoot.rule24K = settingsRoot.kMax
        if (settingsRoot.rule25K > settingsRoot.kMax) settingsRoot.rule25K = settingsRoot.kMax

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

        // ----- Header (title only) -------------------------------------------
        // Back and Save now live in the bottom action bar (footer).
        Text {
            Layout.fillWidth: true
            text: "הגדרות"
            font.pixelSize: 36
            font.bold: true
            color: settingsRoot.textDark
        }

        // ----- Subtitle ------------------------------------------------------
        Text {
            Layout.fillWidth: true
            text: "הפעל או בטל אילוצי שיבוץ קשיחים והגדר את ערך הסף k שלהם."
            color: settingsRoot.textMuted
            font.pixelSize: 14
            wrapMode: Text.WordWrap
        }

        // ----- Global k bound hint -------------------------------------------
        // Bound to kMax so it updates automatically when the exam period length
        // becomes available in Component.onCompleted.
        Text {
            Layout.fillWidth: true
            text: "הערך המקסימלי המותר ל-k: " + settingsRoot.kMax + " (אורך תקופת הבחינות)"
            color: settingsRoot.textMuted
            font.pixelSize: 13
            wrapMode: Text.WordWrap
        }

        // ----- Constraint cards (scrollable) ---------------------------------
        // The ScrollView absorbs the central space and scrolls when the cards do
        // not fit, so the footer below is always anchored and never pushed off
        // screen, whatever the window height is.
        ScrollView {
            id: rulesScroll
            Layout.fillWidth:  true
            Layout.fillHeight: true
            clip: true
            // Match the viewport width so the cards never scroll horizontally.
            contentWidth: availableWidth

            // Inner layout sized to the viewport width so cards fill it.
            ColumnLayout {
                width: rulesScroll.availableWidth
                spacing: 22

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

                        radius: theme.radiusL
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
                            spacing: theme.spacingL

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
                                color: ruleSwitch.checked ? theme.primarySoft : theme.neutralBg

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
                                id: kColumn
                                spacing: 4

                                /**
                                 * Local overflow flag for this rule only.
                                 * Set true when the user manually types a k greater than kMax,
                                 * then cleared once the typed value is valid again or after the
                                 * auto-dismiss timer fires. It is never raised by the silent
                                 * load-time clamp or by programmatic updates, because only the
                                 * contentItem's textEdited signal (a user-only keystroke signal)
                                 * drives it.
                                 */
                                property bool kOverflow: false

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
                                 * editable allows typing a number directly instead of clicking
                                 * the spinner repeatedly. "to: kMax" still clamps the committed
                                 * value, so no invalid k is ever stored. The validator is widened
                                 * beyond kMax on purpose so the user can actually type an
                                 * out-of-range number; the overflow is detected via onTextEdited
                                 * and surfaced in the inline error below before Qt clamps it back.
                                 * onValueChanged fires on every change — buttons, keyboard, and
                                 * programmatic updates from Component.onCompleted. No binding loop
                                 * risk: QML skips the property-change notification when the written
                                 * value is identical to the current one.
                                 */
                                SpinBox {
                                    id: kSpin
                                    editable: true
                                    from:     0
                                    to:       settingsRoot.kMax
                                    value:    settingsRoot[modelData.kProp]
                                    enabled:  ruleSwitch.checked
                                    onValueChanged: {
                                        settingsRoot[modelData.kProp] = value
                                        /**
                                         * A committed value is always clamped to [from, to], so once
                                         * the value actually changes this rule can no longer be in
                                         * overflow. Clear both the per-rule marker and the aggregated
                                         * global state so Save reflects the real (in-range) value.
                                         */
                                        kColumn.kOverflow = false
                                        settingsRoot.setRuleOverflow(index, false)
                                    }

                                    // Permissive validator so out-of-range keystrokes are accepted
                                    // long enough to be detected; the real bound stays "to" above.
                                    validator: IntValidator { bottom: 0; top: 9999 }

                                    /**
                                     * Watches raw keystrokes only (textEdited is a user-only
                                     * signal, never emitted by programmatic value changes), so the
                                     * silent load-time clamp does not raise a false error.
                                     */
                                    Connections {
                                        target: kSpin.contentItem
                                        function onTextEdited() {
                                            var typed = parseInt(kSpin.contentItem.text)
                                            var over  = (!isNaN(typed) && typed > settingsRoot.kMax)

                                            // Per-rule visual marker ("Max is <kMax>") under the SpinBox.
                                            if (over) {
                                                kColumn.kOverflow = true
                                                kOverflowTimer.restart()
                                                // Temporary global reminder above the footer.
                                                settingsRoot.flashOverflowMessage()
                                            } else {
                                                kColumn.kOverflow = false
                                                kOverflowTimer.stop()
                                            }

                                            // Aggregate the real state so Save stays disabled while
                                            // this rule's typed value is out of range.
                                            settingsRoot.setRuleOverflow(index, over)
                                        }
                                    }
                                }

                                /** Auto-dismisses the overflow message after a short delay. */
                                Timer {
                                    id: kOverflowTimer
                                    interval: 2500
                                    onTriggered: kColumn.kOverflow = false
                                }

                                /** Inline error shown only while this rule's typed k exceeds kMax. */
                                Text {
                                    Layout.alignment: Qt.AlignHCenter
                                    visible: kColumn.kOverflow
                                    text: "המקסימום הוא " + settingsRoot.kMax
                                    color: theme.danger
                                    font.pixelSize: 11
                                }
                            }
                        }
                    }
                }
            }
        }

        // ----- Global overflow message (temporary) ---------------------------
        // Shown just above the footer whenever the user types a k above kMax.
        // Auto-dismisses after 5 seconds (overflowMessageTimer); the Save button
        // stays disabled independently for as long as the value is out of range.
        // Hidden items are excluded from the layout, so this reserves no space
        // when not visible.
        Text {
            Layout.fillWidth: true
            visible: settingsRoot.overflowMessageVisible
            text: "שגיאה: הזן מספר קטן מ-" + settingsRoot.kMax
            color: theme.danger
            font.pixelSize: 14
            font.bold: true
            wrapMode: Text.WordWrap
        }

        // ----- Action bar (footer) -------------------------------------------
        // Last child of the main ColumnLayout, so it stays anchored at the
        // bottom and visible while the cards above scroll.
        // Classic UX pattern: Back on the left, primary Save action on the right.
        RowLayout {
            Layout.fillWidth: true
            spacing: theme.spacingL

            /** Back button — pops this screen off the StackView without saving. */
            Button {
                text: "← חזור"
                font.pixelSize: 16
                font.bold: true
                background: Rectangle {
                    color: parent.down    ? theme.neutralBorder
                         : parent.hovered ? theme.neutralBg
                         : "transparent"
                    radius: theme.radiusS
                }
                onClicked: {
                    if (settingsRoot.StackView.view) {
                        settingsRoot.StackView.view.pop()
                    }
                }
            }

            // Spacer that pushes Save to the far right of the action bar.
            Item { Layout.fillWidth: true }

            /**
             * Save button — forwards all ten constraint values to AppController
             * via saveHardConstraints(), then returns to the input screen.
             */
            Button {
                id: saveButton
                text: "שמור"
                font.pixelSize: 16
                font.bold: true
                implicitWidth:  100
                implicitHeight: 46

                // Actively block saving while any rule's k exceeds kMax.
                enabled: !settingsRoot.hasOverflow

                background: Rectangle {
                    // Grayed out and flat when disabled (overflow); primary styling
                    // with hover/press feedback when the value is valid again.
                    color: !saveButton.enabled ? theme.saveDisabledBg
                         : saveButton.down     ? settingsRoot.primaryDark
                         : saveButton.hovered  ? theme.primaryHover
                         : settingsRoot.primary
                    radius: theme.radiusS
                }
                contentItem: Text {
                    text: saveButton.text
                    // Dim the label slightly when the button is disabled.
                    color: saveButton.enabled ? "white" : theme.saveDisabledText
                    font.pixelSize: 16
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment:   Text.AlignVCenter
                }
                onClicked: {
                    /**
                     * Final clamp guard before persisting.
                     * The SpinBox validator is widened to 9999 to let overflow be
                     * detected, so a typed value above kMax can still be sitting in
                     * a rule property. Clamp every k to kMax here so no out-of-bound
                     * value is ever sent to C++, regardless of UI state.
                     */
                    if (settingsRoot.rule21K > settingsRoot.kMax) settingsRoot.rule21K = settingsRoot.kMax
                    if (settingsRoot.rule22K > settingsRoot.kMax) settingsRoot.rule22K = settingsRoot.kMax
                    if (settingsRoot.rule23K > settingsRoot.kMax) settingsRoot.rule23K = settingsRoot.kMax
                    if (settingsRoot.rule24K > settingsRoot.kMax) settingsRoot.rule24K = settingsRoot.kMax
                    if (settingsRoot.rule25K > settingsRoot.kMax) settingsRoot.rule25K = settingsRoot.kMax

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
        }
    }
}
