import QtQuick

/**
 * AppTheme.qml
 *
 * Single source of truth for the app's design language (colors, spacing, radii).
 *
 * Implemented as a plain QtObject (NOT a qmldir singleton) on purpose: this app
 * loads its QML from disk (files copied next to the executable by a CMake
 * POST_BUILD step; no .qrc / qmldir). Each consumer instantiates a local copy:
 *
 *     AppTheme { id: theme }
 *     ... color: theme.primary
 *
 * Structure:
 *   1. CANONICAL PALETTE — the whole DA, ~a dozen real colors.
 *   2. SEMANTIC ALIASES  — the names used by the screens, all pointing back to
 *      the canonical palette so the look is uniform everywhere (one green, white
 *      surfaces, one soft tint, one border). Screens don't need to change when
 *      the palette is tuned: edit a canonical value once.
 */
QtObject {
    // ═══════════════ 1. CANONICAL PALETTE ═══════════════

    // Brand green — the single green used across every screen (chrome + actions).
    readonly property color green:       "#14533f"
    readonly property color greenDark:   "#0f3f30"  // pressed
    readonly property color greenHover:  "#1b664f"  // hover / highlight
    readonly property color greenAccent: "#52B788"  // selection / KPI / calendar accent
    readonly property color greenOnDark: "#A8D5B5"  // light green text over green chrome

    // Surfaces — a normal white, one warm page background, one soft green tint,
    // one muted cool fill.
    readonly property color surface:      "white"
    readonly property color appBg:        "#f7f5ef"
    readonly property color surfaceSoft:  "#edf7f2"
    readonly property color surfaceMuted: "#f1f5f9"

    // Borders — one neutral border, plus a stronger one for emphasis.
    readonly property color border:       "#e2e8f0"
    readonly property color borderStrong: "#cbd5e1"

    // Text greys.
    readonly property color textDark:       "#1f2933"
    readonly property color textMuted:      "#69737a"
    readonly property color textBreadcrumb: "#9aa0a6"
    readonly property color textSteel:      "#475569"

    // Status / state colors — kept distinct because each carries meaning.
    readonly property color danger:          "#b91c1c"
    readonly property color dangerStrong:    "#991b1b"
    readonly property color success:         "#047857"
    readonly property color successText:     "#059669"
    readonly property color disabledGrey:    "#94a3b8"
    readonly property color saveDisabledBg:  "#9aa5a0"
    readonly property color saveDisabledText:"#e2e8e4"
    readonly property color softRedBg:       "#fef2f2"
    readonly property color softRedBorder:   "#fecaca"
    readonly property color cellExcludedText:"#dc2626"
    readonly property color dropValid:       "#86efac"
    readonly property color dropInvalid:     "#fca5a5"
    readonly property color deleteHover:     "#f87171"
    readonly property color statusOkBorder:  "#bbf7d0"

    // ═══════════════ 2. SEMANTIC ALIASES (point to the palette above) ═══════════════

    // Greens — every dark green in the app is now the same brand green.
    readonly property color primary:        green
    readonly property color primaryDark:    greenDark
    readonly property color primaryHover:   greenHover
    readonly property color primarySoft:    surfaceSoft
    readonly property color headerGreen:    green
    readonly property color saveGreen:      green
    readonly property color saveGreenHover: greenHover
    readonly property color accentGreen:    greenAccent
    readonly property color treeYearBg:     greenHover
    readonly property color treeSemBg:      greenHover
    readonly property color sidebarLabel:   greenOnDark
    readonly property color sidebarSemText: greenOnDark

    // Outline-button surface states.
    readonly property color outlineHoverBg: surfaceSoft
    readonly property color outlineDownBg:  surfaceSoft

    // Backgrounds — one page bg, white cards, one soft tint, one muted fill.
    readonly property color pageBg:       appBg
    readonly property color screenBg:     appBg
    readonly property color neutralBg:    surfaceMuted
    readonly property color neutralBgAlt: surfaceMuted
    readonly property color inputBg:      surfaceMuted
    readonly property color selectedBg:   surfaceSoft
    readonly property color statusOkBg:   surfaceSoft
    readonly property color checkIconBg:  surfaceSoft
    readonly property color uploadIconBg: surfaceSoft
    readonly property color cardEmptyBg:  surfaceSoft
    readonly property color cardFilledBg: surface

    // Borders — collapsed to a single neutral border (+ the strong one).
    readonly property color borderSoft:          border
    readonly property color neutralBorder:       border
    readonly property color borderCard:          border
    readonly property color cardEmptyBorder:     border
    readonly property color cardFilledBorder:    border
    readonly property color neutralBorderStrong: borderStrong

    // Near-identical greys merged onto one muted text color.
    readonly property color textSlate:  textMuted
    readonly property color textSubtle: textMuted

    // One error red.
    readonly property color errorRed: danger

    // ═══════════════ Spacing scale ═══════════════
    readonly property int spacingS:  8
    readonly property int spacingM:  12
    readonly property int spacingL:  16
    readonly property int spacingXL: 22
    readonly property int marginL:   34

    // ═══════════════ Corner radii ═══════════════
    readonly property int radiusS:  8
    readonly property int radius:   12
    readonly property int radiusL:  16
    readonly property int radiusXL: 18
}
