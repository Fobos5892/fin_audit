import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FamilyBudget 1.0
import components 1.0

ColumnLayout {
    id: root
    property alias label: labelText.text
    property alias text: field.text
    property alias placeholderText: field.placeholderText
    property alias inputMethodHints: field.inputMethodHints
    property alias validator: field.validator
    property alias readOnly: field.readOnly
    property alias echoMode: field.echoMode
    property string helper: ""
    property bool compact: false
    signal editingFinished()
    signal accepted()

    spacing: root.compact ? Math.max(4, UiTheme.spaceXs * 0.85) : UiTheme.spaceSm
    Layout.fillWidth: true

    Text {
        id: labelText
        visible: text.length > 0
        color: ThemeManager.mutedTextColor
        font.pixelSize: UiTheme.fontSm
        font.bold: true
        Layout.fillWidth: true
        Layout.bottomMargin: root.compact ? 0 : UiTheme.u * 0.15
    }

    TextField {
        id: field
        Layout.fillWidth: true
        Layout.preferredHeight: root.compact ? UiTheme.controlHeight * 0.92 : UiTheme.controlHeight
        font.pixelSize: UiTheme.fontMd
        color: ThemeManager.textColor
        placeholderTextColor: ThemeManager.mutedTextColor
        leftPadding: UiTheme.spaceMd
        rightPadding: UiTheme.spaceMd
        topPadding: UiTheme.spaceSm
        bottomPadding: UiTheme.spaceSm
        verticalAlignment: TextInput.AlignVCenter
        background: Rectangle {
            radius: UiTheme.radiusLg
            color: ThemeManager.isDarkMode ? "#0B1220" : "#F8FAFC"
            border.width: UiTheme.borderWidth
            border.color: field.activeFocus ? ThemeManager.accentColor : ThemeManager.borderColor
            Behavior on border.color { ColorAnimation { duration: 120 } }
        }
        onEditingFinished: root.editingFinished()
        onAccepted: root.accepted()
    }

    Text {
        visible: root.helper.length > 0
        text: root.helper
        color: ThemeManager.mutedTextColor
        font.pixelSize: Math.round(UiTheme.fontSm * 0.92)
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
    }
}
