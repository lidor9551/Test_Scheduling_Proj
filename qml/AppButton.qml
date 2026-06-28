import QtQuick
import QtQuick.Controls

/**
 * AppButton.qml
 *
 * Shared themed button used across all screens.
 *
 * Variant (`variant` property):
 *   "primary" : filled brand-green button (default)
 *   "outline" : white button with a brand-green border and label
 *   "danger"  : filled red button for destructive actions
 *
 * The legacy boolean `outline` is kept for backward compatibility with existing
 * call sites (Main.qml / InputScreen.qml): when true it forces the "outline"
 * variant.
 *
 * implicitWidth/implicitHeight (165 x 46) stay overridable per call site; all
 * colors come from AppTheme so there is no hardcoded hex here.
 *
 * Usage:
 *     AppButton { text: "..."; variant: "danger"; onClicked: ... }
 *     AppButton { text: "..."; outline: true }          // == variant "outline"
 */
Button {
    id: control

    property string variant: "primary"
    // Back-compat alias: `outline: true` behaves exactly like variant "outline".
    property bool outline: false
    readonly property string resolvedVariant: control.outline ? "outline" : control.variant

    implicitWidth: 165
    implicitHeight: 46
    font.pixelSize: 15
    font.bold: true

    AppTheme { id: theme }

    contentItem: Text {
        text: control.text
        color: control.resolvedVariant === "outline" ? theme.primary : "white"
        font: control.font
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
        radius: theme.radius
        border.width: 1
        border.color: control.resolvedVariant === "danger" ? theme.danger : theme.primary
        color: {
            switch (control.resolvedVariant) {
            case "outline":
                return control.down    ? theme.outlineDownBg
                     : control.hovered ? theme.outlineHoverBg
                     : "white"
            case "danger":
                return control.down    ? theme.dangerStrong
                     : control.hovered ? theme.deleteHover
                     : theme.danger
            default: // "primary"
                return control.down    ? theme.primaryDark
                     : control.hovered ? theme.primaryHover
                     : theme.primary
            }
        }
    }
}
