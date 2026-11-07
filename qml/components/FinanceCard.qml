import QtQuick
import QtQuick.Layouts
import FamilyBudget 1.0
import components 1.0

Rectangle {
    id: card
    property string title: ""
    property real cents: 0
    property color accentColor: ThemeManager.accentColor

    // Height from content — do not anchors.fill the column (breaks implicitHeight)
    implicitWidth: Math.max(UiTheme.u * 18, contentCol.implicitWidth + UiTheme.spaceXl * 2)
    implicitHeight: contentCol.implicitHeight + UiTheme.spaceLg * 2
    radius: UiTheme.radiusXl
    color: ThemeManager.surfaceColor
    border.color: ThemeManager.borderColor
    border.width: UiTheme.borderWidth
    clip: true

    scale: mouseArea.containsPress ? 0.98 : (mouseArea.containsMouse ? 1.01 : 1.0)
    Behavior on scale { NumberAnimation { duration: 150; easing.type: Easing.OutQuad } }
    Behavior on color { ColorAnimation { duration: 300 } }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
    }

    Rectangle {
        width: Math.max(3, UiTheme.u * 0.4)
        anchors.left: parent.left
        // Keep clear of the rounded corner
        anchors.leftMargin: Math.max(UiTheme.spaceMd, card.radius * 0.55)
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.topMargin: UiTheme.spaceMd
        anchors.bottomMargin: UiTheme.spaceMd
        color: card.accentColor
        radius: width / 2
        opacity: 0.85
    }

    ColumnLayout {
        id: contentCol
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.leftMargin: UiTheme.spaceXl
        anchors.rightMargin: UiTheme.spaceLg
        anchors.topMargin: UiTheme.spaceLg
        spacing: UiTheme.spaceSm

        Text {
            text: card.title
            font.pixelSize: UiTheme.fontSm
            color: ThemeManager.mutedTextColor
            Layout.fillWidth: true
            elide: Text.ElideRight
        }

        PrivacyAmount {
            cents: card.cents
            valueColor: ThemeManager.textColor
            font.pixelSize: UiTheme.fontXl
            Layout.fillWidth: true
        }
    }
}
