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

        Text {
            text: "Капитал"
            font.pixelSize: UiTheme.fontTitle
            font.bold: true
            color: ThemeManager.textColor
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: UiTheme.spaceMd
            FinanceCard {
                title: "Счета"
                cents: CapitalVM.accountsCents
                accentColor: ThemeManager.accentColor
            }
            FinanceCard {
                title: "Портфель"
                cents: CapitalVM.portfolioCents
                accentColor: ThemeManager.successColor
            }
            FinanceCard {
                title: "Ипотека (−)"
                cents: CapitalVM.mortgageCents
                accentColor: ThemeManager.dangerColor
            }
            FinanceCard {
                title: "Чистый капитал"
                cents: CapitalVM.netWorthCents
                accentColor: ThemeManager.accentColor
            }
        }

        Text {
            text: "Накоплено в целях: " + (ThemeManager.isPrivacyMode ? "••••" : MoneyFormatter.formatCents(CapitalVM.goalsCents))
            color: ThemeManager.mutedTextColor
            font.pixelSize: UiTheme.fontMd
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: UiTheme.radiusXl
            color: ThemeManager.surfaceColor
            border.color: ThemeManager.borderColor
            border.width: UiTheme.borderWidth

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: UiTheme.spaceMd
                spacing: UiTheme.spaceSm
                Text {
                    text: "График роста капитала"
                    font.bold: true
                    font.pixelSize: UiTheme.fontLg
                    color: ThemeManager.textColor
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                }
                Text {
                    text: "Чистый капитал = счета + портфель − остаток ипотеки (база vs досрочка)"
                    color: ThemeManager.mutedTextColor
                    font.pixelSize: UiTheme.fontSm
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                }
                RowLayout {
                    spacing: UiTheme.spaceMd
                    Rectangle {
                        Layout.preferredWidth: UiTheme.spaceSm
                        Layout.preferredHeight: UiTheme.spaceSm
                        radius: UiTheme.u * 0.2
                        color: ThemeManager.accentColor
                    }
                    Text {
                        text: "Без досрочки"
                        color: ThemeManager.mutedTextColor
                        font.pixelSize: UiTheme.fontSm
                    }
                    Rectangle {
                        Layout.preferredWidth: UiTheme.spaceSm
                        Layout.preferredHeight: UiTheme.spaceSm
                        radius: UiTheme.u * 0.2
                        color: ThemeManager.successColor
                    }
                    Text {
                        text: "С досрочкой"
                        color: ThemeManager.mutedTextColor
                        font.pixelSize: UiTheme.fontSm
                    }
                }
                LineChart {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    seriesA: CapitalVM.projectionBase
                    seriesB: CapitalVM.projectionExtra
                    colorA: ThemeManager.accentColor
                    colorB: ThemeManager.successColor
                }
            }
        }
    }

    Component.onCompleted: CapitalVM.refresh()
}
