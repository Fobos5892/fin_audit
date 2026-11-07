import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FamilyBudget 1.0
import components 1.0

Item {
    id: page

    function chartCardHeight() {
        return Math.max(UiTheme.u * 32, page.height * 0.36)
    }

    ScrollView {
        id: pageScroll
        anchors.fill: parent
        anchors.margins: UiTheme.pagePadding
        clip: true
        contentWidth: availableWidth
        ScrollBar.vertical.policy: ScrollBar.AsNeeded

        // No rubber-band / overshoot on the page
        Component.onCompleted: {
            if (contentItem && contentItem.boundsBehavior !== undefined) {
                contentItem.boundsBehavior = Flickable.StopAtBounds
                if (contentItem.rebound !== undefined)
                    contentItem.rebound = null
            }
        }

        ColumnLayout {
            width: pageScroll.availableWidth
            spacing: UiTheme.spaceLg

            Text {
                text: "Дашборд"
                font.pixelSize: UiTheme.fontTitle
                font.bold: true
                color: ThemeManager.textColor
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: UiTheme.spaceMd
                FinanceCard {
                    title: "Баланс счетов"
                    cents: DashboardVM.balanceCents
                    accentColor: ThemeManager.accentColor
                    Layout.fillWidth: true
                }
                FinanceCard {
                    title: "Доходы за месяц"
                    cents: DashboardVM.incomeCents
                    accentColor: ThemeManager.successColor
                    Layout.fillWidth: true
                }
                FinanceCard {
                    title: "Расходы за месяц"
                    cents: DashboardVM.expenseCents
                    accentColor: ThemeManager.dangerColor
                    Layout.fillWidth: true
                }
            }

            // Три графика
            GridLayout {
                Layout.fillWidth: true
                columns: page.width > UiTheme.u * 95 ? 3 : 1
                rowSpacing: UiTheme.spaceMd
                columnSpacing: UiTheme.spaceMd

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: page.chartCardHeight()
                    radius: UiTheme.radiusXl
                    color: ThemeManager.surfaceColor
                    border.color: ThemeManager.borderColor
                    border.width: UiTheme.borderWidth

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: UiTheme.spaceLg
                        spacing: UiTheme.spaceSm
                        Text {
                            text: "Структура доходов"
                            font.bold: true
                            font.pixelSize: UiTheme.fontLg
                            color: ThemeManager.textColor
                            Layout.fillWidth: true
                        }
                        Text {
                            text: "Работа и пассивные источники"
                            color: ThemeManager.mutedTextColor
                            font.pixelSize: UiTheme.fontSm
                            Layout.fillWidth: true
                        }
                        LabeledDonutChart {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            slices: DashboardVM.incomeDonutSlices
                            emptyText: "Нет доходов за месяц"
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: page.chartCardHeight()
                    radius: UiTheme.radiusXl
                    color: ThemeManager.surfaceColor
                    border.color: ThemeManager.borderColor
                    border.width: UiTheme.borderWidth

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: UiTheme.spaceLg
                        spacing: UiTheme.spaceSm
                        Text {
                            text: "Структура расходов"
                            font.bold: true
                            font.pixelSize: UiTheme.fontLg
                            color: ThemeManager.textColor
                            Layout.fillWidth: true
                        }
                        Text {
                            text: "По категориям за месяц"
                            color: ThemeManager.mutedTextColor
                            font.pixelSize: UiTheme.fontSm
                            Layout.fillWidth: true
                        }
                        LabeledDonutChart {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            slices: DashboardVM.expenseDonutSlices
                            emptyText: "Нет расходов за месяц"
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: page.chartCardHeight()
                    radius: UiTheme.radiusXl
                    color: ThemeManager.surfaceColor
                    border.color: ThemeManager.borderColor
                    border.width: UiTheme.borderWidth

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: UiTheme.spaceLg
                        spacing: UiTheme.spaceSm
                        Text {
                            text: "Доходы / расходы"
                            font.bold: true
                            font.pixelSize: UiTheme.fontLg
                            color: ThemeManager.textColor
                            Layout.fillWidth: true
                        }
                        Text {
                            text: "Расходы от доходов · бюджет = работа + пассив"
                            color: ThemeManager.mutedTextColor
                            font.pixelSize: UiTheme.fontSm
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                        }
                        BudgetRingChart {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            workCents: DashboardVM.workIncomeCents
                            passiveCents: DashboardVM.passiveIncomeCents
                            expenseCents: DashboardVM.expenseCents
                        }
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: UiTheme.spaceSm
                            Rectangle {
                                Layout.preferredWidth: UiTheme.spaceSm
                                Layout.preferredHeight: UiTheme.spaceSm
                                radius: width / 2
                                color: "#7C6BC4"
                            }
                            Text {
                                text: "Работа"
                                color: ThemeManager.mutedTextColor
                                font.pixelSize: UiTheme.fontSm
                            }
                            Rectangle {
                                Layout.preferredWidth: UiTheme.spaceSm
                                Layout.preferredHeight: UiTheme.spaceSm
                                radius: width / 2
                                color: "#5B8FD9"
                            }
                            Text {
                                text: "Пассив"
                                color: ThemeManager.mutedTextColor
                                font.pixelSize: UiTheme.fontSm
                            }
                            Item { Layout.fillWidth: true }
                            Text {
                                text: Math.round(DashboardVM.expenseToIncomePct) + "% расходов"
                                color: DashboardVM.expenseToIncomePct <= 100
                                       ? ThemeManager.successColor
                                       : ThemeManager.dangerColor
                                font.pixelSize: UiTheme.fontSm
                                font.bold: true
                            }
                        }
                    }
                }
            }

            // Последние операции: доходы слева, расходы справа
            GridLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: Math.max(UiTheme.u * 40, page.height * 0.42) * (columns === 1 ? 2 : 1)
                columns: page.width > UiTheme.u * 75 ? 2 : 1
                rowSpacing: UiTheme.spaceMd
                columnSpacing: UiTheme.spaceMd

                TransactionMiniList {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    title: "Последние доходы"
                    model: DashboardVM.incomeTransactions
                    emptyText: "Нет доходов"
                    accent: ThemeManager.successColor
                }
                TransactionMiniList {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    title: "Последние расходы"
                    model: DashboardVM.expenseTransactions
                    emptyText: "Нет расходов"
                    accent: ThemeManager.dangerColor
                }
            }

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: goalsCol.implicitHeight + UiTheme.spaceLg * 2
                radius: UiTheme.radiusXl
                color: ThemeManager.surfaceColor
                border.color: ThemeManager.borderColor
                border.width: UiTheme.borderWidth

                ColumnLayout {
                    id: goalsCol
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: UiTheme.spaceLg
                    spacing: UiTheme.spaceLg
                    Text {
                        text: "Цели"
                        font.bold: true
                        font.pixelSize: UiTheme.fontLg
                        color: ThemeManager.textColor
                    }
                    Repeater {
                        model: DashboardVM.goals
                        delegate: Item {
                            id: goalWrap
                            Layout.fillWidth: true
                            implicitHeight: bar.implicitHeight
                            required property string name
                            required property real progress
                            required property real currentCents
                            required property real targetCents
                            GoalProgressBar {
                                id: bar
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.top: parent.top
                                title: goalWrap.name
                                progressPct: goalWrap.progress
                                currentCents: goalWrap.currentCents
                                targetCents: goalWrap.targetCents
                            }
                        }
                    }
                }
            }
        }
    }
}
