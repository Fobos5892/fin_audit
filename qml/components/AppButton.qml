import QtQuick
import QtQuick.Controls
import FamilyBudget 1.0
import components 1.0

Button {
    id: root
    property bool primary: false
    property bool danger: false

    font.pixelSize: UiTheme.fontMd
    font.bold: true
    leftPadding: UiTheme.spaceMd
    rightPadding: UiTheme.spaceMd
    topPadding: UiTheme.spaceSm
    bottomPadding: UiTheme.spaceSm
    implicitHeight: UiTheme.controlHeight
    implicitWidth: Math.max(UiTheme.buttonMinWidth,
                            contentItem.implicitWidth + leftPadding + rightPadding)

    contentItem: Text {
        text: root.text
        font: root.font
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: {
            if (!root.enabled)
                return ThemeManager.mutedTextColor
            if (root.danger)
                return ThemeManager.dangerColor
            if (root.primary)
                return "#F8FAFC"
            return ThemeManager.selectionTextColor
        }
        opacity: root.enabled ? 1.0 : 0.55
    }

    background: Rectangle {
        // Pill-like corners (example UI), still within theme palette
        radius: Math.min(height / 2, UiTheme.radiusXl)
        color: {
            if (root.primary) {
                if (root.down)
                    return Qt.darker(ThemeManager.accentColor, 1.18)
                if (root.hovered)
                    return Qt.lighter(ThemeManager.accentColor, 1.1)
                return ThemeManager.accentColor
            }
            if (root.danger) {
                if (root.down)
                    return ThemeManager.isDarkMode ? "#3A1F24" : "#FEE2E2"
                if (root.hovered)
                    return ThemeManager.isDarkMode ? "#2A181C" : "#FEF2F2"
                return ThemeManager.surfaceColor
            }
            if (root.down)
                return ThemeManager.isDarkMode ? "#243447" : "#E2E8F0"
            if (root.hovered)
                return ThemeManager.isDarkMode ? "#1E293B" : "#F1F5F9"
            return ThemeManager.surfaceColor
        }
        border.width: UiTheme.borderWidth
        border.color: {
            if (root.danger)
                return ThemeManager.dangerColor
            if (root.primary)
                return Qt.darker(ThemeManager.accentColor, 1.15)
            return ThemeManager.selectionBorderColor
        }
        Behavior on color { ColorAnimation { duration: 120 } }
        Behavior on border.color { ColorAnimation { duration: 120 } }
    }
}
