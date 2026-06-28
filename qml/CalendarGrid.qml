import QtQuick
import QtQuick.Layouts

/**
 * CalendarGrid.qml
 *
 * Shared 7-column (one week per row) calendar grid, extracted from the identical
 * inline grids previously duplicated in CalendarScreen.qml and
 * PeriodEditorScreen.qml.
 *
 * This is a pure extraction: rendering, cell size, RTL direction and the toggle
 * behaviour are byte-for-byte equivalent to the previous inline grids. Anything
 * that differed between the two screens is exposed here:
 *
 *  - `model`          : the padded day model (each entry exposes { date, status };
 *                       status -1 = padding cell, hidden; status 2 = excluded day).
 *                       The owning screen builds and refreshes it (and updates its
 *                       own month title) exactly as before.
 *  - `dayToggled()`   : emitted when an allowed day cell is clicked; the owner
 *                       wires it to calendarManager.toggleDay(dateStr).
 *  - state colors     : exposed as properties whose defaults reproduce the
 *                       current hardcoded values, so the look is unchanged.
 *
 * Note: this component is intentionally NOT wired into the two screens in this
 * change set; that is done in the follow-up steps.
 */
GridView {
    id: calendarGrid

    /** Emitted when an allowed day cell is clicked. dateStr is "yyyy-MM-dd". */
    signal dayToggled(string dateStr)

    AppTheme { id: theme }

    // ── State colors (defaults match the previous inline values exactly) ──────
    property color excludedCellColor:   theme.softRedBg
    property color normalCellColor:     theme.neutralBg
    property color excludedBorderColor: theme.softRedBorder
    property color normalBorderColor:   theme.neutralBorder
    property color excludedTextColor:   theme.cellExcludedText
    property color normalTextColor:     theme.textSlate

    Layout.fillWidth: true
    Layout.fillHeight: true
    cellWidth:  Math.floor(width / 7)
    cellHeight: 120
    layoutDirection: Qt.RightToLeft

    /*
     * Delegate for a single calendar day cell.
     */
    delegate: Rectangle {
        width:  calendarGrid.cellWidth  - 10
        height: calendarGrid.cellHeight - 10
        radius: theme.radiusS
        visible: modelData.status !== -1

        /*
         * Cell background and border reflect whether the day is excluded.
         */
        color: modelData.status === 2 ? calendarGrid.excludedCellColor
                                      : calendarGrid.normalCellColor
        border.color: modelData.status === 2 ? calendarGrid.excludedBorderColor
                                             : calendarGrid.normalBorderColor
        border.width: 1

        /*
         * Calendar day number.
         */
        Text {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 8
            text: modelData.date ? Qt.formatDate(modelData.date, "d") : ""
            font.pixelSize: 14
            font.bold: true
            color: modelData.status === 2 ? calendarGrid.excludedTextColor
                                          : calendarGrid.normalTextColor
        }

        /*
         * Clicking an allowed cell toggles the day status.
         * Saturday (getDay() === 6) is disabled and cannot be toggled.
         */
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            enabled: modelData.status !== -1 && modelData.date.getDay() !== 6
            onClicked: {
                let dateStr = Qt.formatDate(modelData.date, "yyyy-MM-dd")
                calendarGrid.dayToggled(dateStr)
            }
        }
    }
}
