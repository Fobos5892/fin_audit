import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FamilyBudget 1.0
import components 1.0

Rectangle {
    id: root
    property string title: ""
    property var model
    property string emptyText: "Нет операций"
    property color accent: ThemeManager.accentColor
    property bool showRemove: false
    signal removeRequested(int id)

    radius: UiTheme.radiusXl
    color: ThemeManager.surfaceColor
    border.color: ThemeManager.borderColor
    border.width: UiTheme.borderWidth

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: UiTheme.spaceLg
        spacing: UiTheme.spaceMd

        RowLayout {
            Layout.fillWidth: true
            spacing: UiTheme.spaceSm

            Rectangle {
                Layout.preferredWidth: Math.max(3, UiTheme.u * 0.4)
                Layout.preferredHeight: titleText.implicitHeight
                radius: width / 2
                color: root.accent
            }
            Text {
                id: titleText
                text: root.title
                font.bold: true
                font.pixelSize: UiTheme.fontLg
                color: ThemeManager.textColor
                Layout.fillWidth: true
                elide: Text.ElideRight
            }
            Text {
                text: list.count
                color: ThemeManager.mutedTextColor
                font.pixelSize: UiTheme.fontSm
                font.bold: true
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id: list
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: scrollBar.visible ? scrollBar.left : parent.right
                anchors.rightMargin: scrollBar.visible ? UiTheme.spaceSm : 0
                clip: true
                spacing: UiTheme.spaceSm
                model: root.model
                boundsBehavior: Flickable.StopAtBounds
                ScrollBar.vertical: scrollBar

                WheelHandler {
                    acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
                    onWheel: function(event) {
                        var dy = event.pixelDelta.y !== 0 ? -event.pixelDelta.y : -event.angleDelta.y / 4
                        var maxY = Math.max(0, list.contentHeight - list.height)
                        var next = Math.max(0, Math.min(maxY, list.contentY + dy))
                        event.accepted = next !== list.contentY
                        if (event.accepted)
                            list.contentY = next
                    }
                }

                Text {
                    anchors.centerIn: parent
                    visible: list.count === 0
                    text: root.emptyText
                    color: ThemeManager.mutedTextColor
                    font.pixelSize: UiTheme.fontMd
                }

                delegate: Item {
                    id: txWrap
                    width: list.width
                    height: Math.max(UiTheme.controlHeight * 1.5, txInner.implicitHeight + UiTheme.spaceMd * 2)
                    required property int id
                    required property string categoryName
                    required property string description
                    required property string ownership
                    required property string chartColor
                    required property real amountCents

                    Rectangle {
                        anchors.fill: parent
                        radius: UiTheme.radiusLg
                        color: ThemeManager.isDarkMode ? "#243044" : "#F1F5F9"

                        RowLayout {
                            id: txInner
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.leftMargin: UiTheme.spaceMd
                            anchors.rightMargin: UiTheme.spaceMd
                            spacing: UiTheme.spaceMd

                            Rectangle {
                                Layout.preferredWidth: Math.max(8, UiTheme.u * 0.9)
                                Layout.preferredHeight: Layout.preferredWidth
                                Layout.alignment: Qt.AlignVCenter
                                radius: width / 2
                                color: txWrap.chartColor || ThemeManager.accentColor
                            }

                            ColumnLayout {
                                Layout.fillWidth: true
                                Layout.alignment: Qt.AlignVCenter
                                spacing: Math.max(2, UiTheme.u * 0.25)

                                Text {
                                    text: txWrap.categoryName || "Без категории"
                                    color: ThemeManager.textColor
                                    font.bold: true
                                    font.pixelSize: UiTheme.fontMd
                                    elide: Text.ElideRight
                                    Layout.fillWidth: true
                                }
                                Text {
                                    text: txWrap.description
                                          || MoneyFormatter.ownershipLabel(txWrap.ownership)
                                    color: ThemeManager.mutedTextColor
                                    font.pixelSize: UiTheme.fontSm
                                    elide: Text.ElideRight
                                    Layout.fillWidth: true
                                }
                            }

                            PrivacyAmount {
                                cents: txWrap.amountCents
                                showSign: true
                                font.pixelSize: UiTheme.fontMd
                                Layout.alignment: Qt.AlignVCenter
                                valueColor: txWrap.amountCents >= 0 ? ThemeManager.successColor
                                                                    : ThemeManager.dangerColor
                            }

                            AppButton {
                                visible: root.showRemove
                                text: "✕"
                                danger: true
                                Layout.preferredWidth: UiTheme.controlHeight
                                Layout.preferredHeight: UiTheme.controlHeight * 0.85
                                leftPadding: 0
                                rightPadding: 0
                                Layout.alignment: Qt.AlignVCenter
                                onClicked: root.removeRequested(txWrap.id)
                            }
                        }
                    }
                }
            }

            ScrollBar {
                id: scrollBar
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                width: Math.max(6, UiTheme.u * 0.7)
                policy: ScrollBar.AsNeeded
                visible: list.contentHeight > list.height
                contentItem: Rectangle {
                    implicitWidth: scrollBar.width
                    radius: width / 2
                    color: ThemeManager.mutedTextColor
                    opacity: scrollBar.pressed ? 0.9 : (scrollBar.hovered ? 0.7 : 0.45)
                }
                background: Rectangle {
                    radius: width / 2
                    color: ThemeManager.borderColor
                    opacity: 0.5
                }
            }
        }
    }
}
