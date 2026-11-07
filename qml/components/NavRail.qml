import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FamilyBudget 1.0
import components 1.0

Rectangle {
    id: root
    property int currentIndex: 0
    property bool compact: false
    signal navigated(int index)

    color: ThemeManager.surfaceColor
    Behavior on color { ColorAnimation { duration: 300 } }

    readonly property var items: [
        { title: "Дашборд", icon: "⌂" },
        { title: "Операции", icon: "⇄" },
        { title: "Цели", icon: "◎" },
        { title: "Инвестиции", icon: "↗" },
        { title: "Ипотека", icon: "⌂+" },
        { title: "Капитал", icon: "◈" },
        { title: "Настройки", icon: "⚙" }
    ]

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: root.compact ? UiTheme.spaceSm : UiTheme.spaceMd
        spacing: UiTheme.spaceXs

        RowLayout {
            visible: !SettingsStore.hideChromeForRecording
            Layout.fillWidth: true
            Layout.bottomMargin: UiTheme.spaceSm
            spacing: UiTheme.spaceSm

            Image {
                source: "qrc:/resources/icons/app_icon_64.png"
                Layout.preferredWidth: root.compact ? UiTheme.iconSm : UiTheme.iconMd
                Layout.preferredHeight: Layout.preferredWidth
                fillMode: Image.PreserveAspectFit
                smooth: true
                mipmap: true
            }

            Text {
                visible: !root.compact
                text: "FAMILY COIN"
                font.pixelSize: UiTheme.fontLg
                font.bold: true
                color: ThemeManager.textColor
                Layout.fillWidth: true
                elide: Text.ElideRight
            }
        }

        Repeater {
            model: root.items
            delegate: Button {
                id: navBtn
                required property int index
                required property var modelData
                Layout.fillWidth: true
                Layout.preferredHeight: UiTheme.controlHeight
                flat: true
                text: root.compact ? modelData.icon : (modelData.icon + "  " + modelData.title)
                font.pixelSize: UiTheme.fontMd
                contentItem: Text {
                    text: navBtn.text
                    font.pixelSize: UiTheme.fontMd
                    color: root.currentIndex === navBtn.index
                           ? ThemeManager.selectionTextColor
                           : ThemeManager.mutedTextColor
                    font.bold: root.currentIndex === navBtn.index
                    horizontalAlignment: root.compact ? Text.AlignHCenter : Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    leftPadding: root.compact ? 0 : UiTheme.spaceSm
                }
                background: Rectangle {
                    radius: Math.min(height / 2, UiTheme.radiusXl)
                    color: root.currentIndex === navBtn.index
                           ? ThemeManager.selectionBgColor
                           : (navBtn.hovered ? (ThemeManager.isDarkMode ? "#243041" : "#F1F5F9")
                                             : "transparent")
                    border.width: root.currentIndex === navBtn.index ? UiTheme.borderWidth : 0
                    border.color: ThemeManager.selectionBorderColor

                    Rectangle {
                        visible: root.currentIndex === navBtn.index
                        width: Math.max(2, UiTheme.u * 0.28)
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.margins: Math.max(2, UiTheme.u * 0.25)
                        radius: width / 2
                        color: ThemeManager.accentColor
                        opacity: 0.75
                    }
                }
                onClicked: {
                    root.currentIndex = navBtn.index
                    root.navigated(navBtn.index)
                }
            }
        }

        Item { Layout.fillHeight: true }

        RowLayout {
            visible: !SettingsStore.hideChromeForRecording
            Layout.fillWidth: true
            spacing: UiTheme.spaceXs

            AppButton {
                id: privacyBtn
                Layout.preferredWidth: UiTheme.controlHeight
                Layout.preferredHeight: UiTheme.controlHeight
                leftPadding: 0
                rightPadding: 0
                font.pixelSize: UiTheme.fontLg
                // High-contrast eye; slash overlay when amounts are hidden
                contentItem: Item {
                    Text {
                        anchors.centerIn: parent
                        text: "👁"
                        font.pixelSize: UiTheme.fontLg
                        color: ThemeManager.selectionTextColor
                    }
                    Rectangle {
                        visible: ThemeManager.isPrivacyMode
                        width: parent.width * 0.62
                        height: Math.max(2, UiTheme.u * 0.22)
                        radius: height / 2
                        anchors.centerIn: parent
                        rotation: -28
                        color: ThemeManager.selectionTextColor
                    }
                }
                onClicked: ThemeManager.isPrivacyMode = !ThemeManager.isPrivacyMode
                ToolTip.visible: hovered
                ToolTip.text: ThemeManager.isPrivacyMode ? "Показать суммы" : "Скрыть суммы"
            }
            AppButton {
                text: ThemeManager.isDarkMode ? "☀" : "☾"
                Layout.preferredWidth: UiTheme.controlHeight
                Layout.preferredHeight: UiTheme.controlHeight
                leftPadding: 0
                rightPadding: 0
                font.pixelSize: UiTheme.fontLg
                onClicked: SettingsVM.isDarkMode = !SettingsVM.isDarkMode
            }
        }
    }
}
