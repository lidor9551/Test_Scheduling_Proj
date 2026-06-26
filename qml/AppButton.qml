import QtQuick
import QtQuick.Controls

/**
 * AppButton.qml
 *
 * Shared primary / outline button, extracted verbatim from the inline component
 * that used to live in Main.qml. Appearance, sizes and behaviour are unchanged;
 * the palette colors now come from AppTheme instead of the Main.qml root.
 *
 * Usage:
 *     AppButton { text: "..."; outline: true; onClicked: ... }
 */
Button {
    id: control

    property bool outline: false

    implicitWidth: 165
    implicitHeight: 46
    font.pixelSize: 15
    font.bold: true

    AppTheme { id: theme }

    contentItem: Text {
        text: control.text
        color: control.outline ? theme.primary : "white"
        font.pixelSize: 15
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
        radius: 12
        color: control.outline
               ? (control.down ? "#e3efe9" : control.hovered ? "#f3faf6" : "white")
               : (control.down ? theme.primaryDark : control.hovered ? "#1b664f" : theme.primary)
        border.color: theme.primary
        border.width: 1
    }
}
