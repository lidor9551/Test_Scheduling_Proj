import QtQuick

/**
 * AppTheme.qml
 *
 * Centralized color palette shared across all screens.
 *
 * Implemented as a plain QtObject (NOT a qmldir singleton) on purpose: this app
 * loads its QML from disk (the files are copied next to the executable by a
 * CMake POST_BUILD step; there is no .qrc and no qmldir). A file-based qmldir
 * singleton would not be copied and would fail to import at runtime, so each
 * consumer instantiates a local instance instead:
 *
 *     AppTheme { id: theme }
 *     ... color: theme.primary
 *
 * The values are the exact hex codes that were previously duplicated in
 * Main.qml, OutputScreen.qml and SettingsScreen.qml — no visual change.
 */
QtObject {
    readonly property color primary:     "#14533f"
    readonly property color primaryDark: "#0f3f30"
    readonly property color primarySoft: "#edf7f2"
    readonly property color borderSoft:  "#e1e5df"
    readonly property color textDark:    "#1f2933"
    readonly property color textMuted:   "#69737a"
    readonly property color danger:      "#b91c1c"
    readonly property color success:     "#047857"
    readonly property color pageBg:      "#f7f5ef"
}
