import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FamilyBudget 1.0
import components 1.0

Dialog {
    id: root
    modal: true
    anchors.centerIn: Overlay.overlay
    padding: UiTheme.spaceLg
    topPadding: UiTheme.spaceMd
    bottomPadding: UiTheme.spaceMd
    leftPadding: UiTheme.spaceLg
    rightPadding: UiTheme.spaceLg
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    // Let nested ComboBox popups paint outside the dialog card
    clip: false

    property bool showFooterButtons: true
    property string acceptText: "OK"
    property string rejectText: "Отмена"

    header: Item {
        implicitHeight: root.title.length > 0 ? titleLabel.implicitHeight + UiTheme.spaceMd : 0
        width: root.availableWidth
        visible: root.title.length > 0
        Text {
            id: titleLabel
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            text: root.title
            color: ThemeManager.textColor
            font.pixelSize: UiTheme.fontLg
            font.bold: true
            elide: Text.ElideRight
        }
    }

    background: Rectangle {
        radius: UiTheme.radiusXl
        color: ThemeManager.surfaceColor
        border.width: UiTheme.borderWidth
        border.color: ThemeManager.borderColor
    }

    footer: Item {
        visible: root.showFooterButtons
        implicitHeight: visible ? btnRow.implicitHeight + UiTheme.spaceMd * 2 : 0
        width: root.width

        RowLayout {
            id: btnRow
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: UiTheme.spaceLg
            anchors.leftMargin: UiTheme.spaceLg
            spacing: UiTheme.spaceSm

            AppButton {
                text: root.rejectText
                onClicked: root.reject()
            }
            AppButton {
                text: root.acceptText
                primary: true
                onClicked: root.accept()
            }
        }
    }
}
