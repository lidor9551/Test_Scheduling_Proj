import QtQuick

/**
 * AppTheme.qml
 *
 * Centralized design tokens (colors, spacing, radii) shared across every screen.
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
 * Every value below is the exact token that was previously hardcoded across the
 * screens, so centralizing them produces no visual change.
 */
QtObject {
    // ── Brand green (primary action) ─────────────────────────────────────────
    readonly property color primary:        "#14533f"
    readonly property color primaryDark:    "#0f3f30"
    readonly property color primaryHover:   "#1b664f"
    readonly property color primarySoft:    "#edf7f2"

    // ── Outline-button surface states ────────────────────────────────────────
    readonly property color outlineDownBg:  "#e3efe9"
    readonly property color outlineHoverBg: "#f3faf6"

    // ── Header / sidebar / tree chrome (darker green family) ─────────────────
    readonly property color headerGreen:    "#1B4332"
    readonly property color treeYearBg:     "#2D6A4F"
    readonly property color treeSemBg:      "#3A7D5E"
    readonly property color accentGreen:    "#52B788"
    readonly property color saveGreen:      "#157347"
    readonly property color saveGreenHover: "#1a6b45"
    readonly property color sidebarLabel:   "#A8D5B5"
    readonly property color sidebarSemText: "#D4EDD9"

    // ── Text ─────────────────────────────────────────────────────────────────
    readonly property color textDark:       "#1f2933"
    readonly property color textMuted:      "#69737a"
    readonly property color textSlate:      "#64748b"
    readonly property color textSubtle:     "#6C757D"
    readonly property color textBreadcrumb: "#9aa0a6"
    readonly property color textSteel:      "#475569"

    // ── Surfaces / neutrals ──────────────────────────────────────────────────
    readonly property color pageBg:             "#f7f5ef"
    readonly property color screenBg:           "#FAF8F3"
    readonly property color neutralBg:          "#f1f5f9"
    readonly property color neutralBgAlt:       "#f8fafc"
    readonly property color neutralBorder:      "#e2e8f0"
    readonly property color neutralBorderStrong:"#cbd5e1"
    readonly property color borderSoft:         "#e1e5df"
    readonly property color borderCard:         "#E9ECEF"
    readonly property color inputBg:            "#F8F9FA"

    // ── Status / semantic ────────────────────────────────────────────────────
    readonly property color danger:         "#b91c1c"
    readonly property color dangerStrong:   "#991b1b"
    readonly property color success:        "#047857"
    readonly property color successText:    "#059669"
    readonly property color errorRed:       "#C0392B"
    readonly property color disabledGrey:   "#94a3b8"
    readonly property color saveDisabledBg: "#9aa5a0"
    readonly property color saveDisabledText:"#e2e8e4"

    // ── Calendar cell / drag-drop states ─────────────────────────────────────
    readonly property color softRedBg:        "#fef2f2"
    readonly property color softRedBorder:    "#fecaca"
    readonly property color cellExcludedText: "#dc2626"
    readonly property color dropValid:        "#86efac"
    readonly property color dropInvalid:      "#fca5a5"
    readonly property color deleteHover:      "#f87171"

    // ── Input screen accents (program selection / file cards / banners) ──────
    readonly property color selectedBg:     "#f0fdf4"
    readonly property color checkIconBg:    "#dcfce7"
    readonly property color uploadIconBg:   "#e7f3ee"
    readonly property color cardEmptyBg:    "#f2f7f5"
    readonly property color cardFilledBg:   "#f8faf9"
    readonly property color cardEmptyBorder:"#d3ddd8"
    readonly property color cardFilledBorder:"#cbded5"
    readonly property color statusOkBg:     "#ecfdf5"
    readonly property color statusOkBorder: "#bbf7d0"

    // ── Spacing scale (most frequent existing values) ────────────────────────
    readonly property int spacingS:  8
    readonly property int spacingM:  12
    readonly property int spacingL:  16
    readonly property int spacingXL: 22
    readonly property int marginL:   34

    // ── Corner radii ─────────────────────────────────────────────────────────
    readonly property int radiusS:  8
    readonly property int radius:    12
    readonly property int radiusL:  16
    readonly property int radiusXL: 18
}
