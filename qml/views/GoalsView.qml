import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FamilyBudget 1.0
import components 1.0

Item {
    id: page

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: UiTheme.pagePadding
        spacing: UiTheme.spaceMd

        RowLayout {
            Layout.fillWidth: true
            spacing: UiTheme.spaceSm

            Text {
                text: "Финансовые цели"
                font.pixelSize: UiTheme.fontTitle
                font.bold: true
                color: ThemeManager.textColor
                Layout.fillWidth: true
                elide: Text.ElideRight
            }

            RowLayout {
                spacing: UiTheme.spaceXs
                PrivacyAmount {
                    cents: GoalVM.freeCashCents
                    font.pixelSize: UiTheme.fontLg
                }
                Text {
                    text: "свободно"
                    color: ThemeManager.mutedTextColor
                    font.pixelSize: UiTheme.fontSm
                }
            }

            AppButton {
                text: "Авто 20%"
                onClicked: GoalVM.autoAllocatePercent(20)
            }
            AppButton {
                text: "Новая цель"
                primary: true
                onClicked: addGoal.open()
            }
        }

        ListView {
            id: list
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: UiTheme.spaceMd
            model: GoalVM.goals
            delegate: Item {
                id: goalRow
                width: list.width
                height: card.implicitHeight
                required property int id
                required property string name
                required property real progress
                required property real currentCents
                required property real targetCents
                required property bool autoAllocate

                Rectangle {
                    id: card
                    anchors.left: parent.left
                    anchors.right: parent.right
                    implicitHeight: cardCol.implicitHeight + UiTheme.spaceLg * 2
                    radius: UiTheme.radiusXl
                    color: ThemeManager.surfaceColor
                    border.color: ThemeManager.borderColor
                    border.width: UiTheme.borderWidth

                    ColumnLayout {
                        id: cardCol
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.margins: UiTheme.spaceLg
                        spacing: UiTheme.spaceMd

                        GoalProgressBar {
                            Layout.fillWidth: true
                            title: goalRow.name
                            progressPct: goalRow.progress
                            currentCents: goalRow.currentCents
                            targetCents: goalRow.targetCents
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: UiTheme.spaceSm

                            Text {
                                text: goalRow.autoAllocate ? "Режим: авто" : "Режим: вручную"
                                color: ThemeManager.mutedTextColor
                                font.pixelSize: UiTheme.fontSm
                                Layout.alignment: Qt.AlignVCenter
                            }

                            Item { Layout.fillWidth: true }

                            LabeledField {
                                id: addField
                                label: "Сумма пополнения, ₽"
                                placeholderText: "0"
                                compact: true
                                Layout.preferredWidth: Math.max(UiTheme.u * 18, list.width * 0.22)
                                Layout.fillWidth: false
                                inputMethodHints: Qt.ImhFormattedNumbersOnly
                            }

                            AppButton {
                                text: "Пополнить"
                                primary: true
                                Layout.alignment: Qt.AlignBottom
                                onClicked: {
                                    GoalVM.contribute(goalRow.id, addField.text)
                                    addField.text = ""
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    AppDialog {
        id: addGoal
        title: "Новая цель"
        width: Math.min(page.width * 0.55, UiTheme.u * 55)

        ColumnLayout {
            width: addGoal.availableWidth
            spacing: UiTheme.spaceMd

            LabeledField {
                id: nameField
                label: "Название цели"
                placeholderText: "Например, Подушка безопасности"
                Layout.fillWidth: true
            }
            LabeledField {
                id: targetField
                label: "Целевая сумма, ₽"
                placeholderText: "Сколько нужно накопить"
                helper: "Итоговая сумма цели"
                Layout.fillWidth: true
                inputMethodHints: Qt.ImhFormattedNumbersOnly
            }
            LabeledField {
                id: currentField
                label: "Уже накоплено, ₽"
                placeholderText: "0"
                helper: "Текущий прогресс на старте"
                Layout.fillWidth: true
                inputMethodHints: Qt.ImhFormattedNumbersOnly
            }
            CheckBox {
                id: autoBox
                text: "Автоматически распределять свободные средства"
                contentItem: Text {
                    text: parent.text
                    color: ThemeManager.textColor
                    font.pixelSize: UiTheme.fontMd
                    leftPadding: parent.indicator.width + UiTheme.spaceSm
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
                }
            }
        }
        onAccepted: GoalVM.addGoal(nameField.text, targetField.text, currentField.text, "", autoBox.checked)
    }
}
