import QtQuick
import QtQuick.Window
import QtQuick.Controls

/**
 * Main.qml — application window and screen host.
 *
 * Keeps only the top-level window, the StackView that hosts the screens, and
 * global wiring. The home screen (data input + program selection) lives in
 * InputScreen.qml; every other screen is pushed onto the StackView from there.
 */
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

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: InputScreen {}
    }
}
