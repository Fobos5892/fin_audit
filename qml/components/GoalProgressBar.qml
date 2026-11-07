import QtQuick
import QtQuick.Layouts
import FamilyBudget 1.0
import components 1.0

ColumnLayout {
    id: root
    property string title: ""
    property real progressPct: 0
    property real currentCents: 0
    property real targetCents: 0
    spacing: UiTheme.spaceSm

    RowLayout {
        Layout.fillWidth: true
        spacing: UiTheme.spaceSm
        Text {
            text: root.title
            color: ThemeManager.textColor
            font.bold: true
            font.pixelSize: UiTheme.fontLg
            Layout.fillWidth: true
            elide: Text.ElideRight
        }
        Text {
            text: Math.min(100, Math.round(root.progressPct)) + "%"
            color: ThemeManager.accentColor
            font.bold: true
            font.pixelSize: UiTheme.fontMd
        }
    }

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: UiTheme.progressHeight
        radius: height / 2
        color: ThemeManager.borderColor

        Rectangle {
            width: parent.width * Math.min(1.0, root.progressPct / 100.0)
            height: parent.height
            radius: height / 2
            color: ThemeManager.accentColor
            Behavior on width { NumberAnimation { duration: 400; easing.type: Easing.OutCubic } }
        }
    }

    RowLayout {
        Layout.fillWidth: true
        spacing: UiTheme.spaceSm
        PrivacyAmount {
            cents: root.currentCents
            font.pixelSize: UiTheme.fontSm
            font.bold: false
            valueColor: ThemeManager.mutedTextColor
        }
        Item { Layout.fillWidth: true }
        PrivacyAmount {
            cents: root.targetCents
            font.pixelSize: UiTheme.fontSm
            font.bold: false
            valueColor: ThemeManager.mutedTextColor
        }
    }
}
