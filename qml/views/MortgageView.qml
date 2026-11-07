import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FamilyBudget 1.0
import components 1.0

Item {
    id: page

    ScrollView {
        id: scroller
        anchors.fill: parent
        anchors.margins: UiTheme.pagePadding
        clip: true
        contentWidth: availableWidth

        ColumnLayout {
            width: scroller.availableWidth
            spacing: UiTheme.spaceLg

            RowLayout {
                Layout.fillWidth: true
                spacing: UiTheme.spaceSm

                Text {
                    text: "Ипотека"
                    font.pixelSize: UiTheme.fontTitle
                    font.bold: true
                    color: ThemeManager.textColor
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                }
                AppButton {
                    text: "Пересчитать"
                    onClicked: MortgageVM.recalculate()
                }
                AppButton {
                    text: "Сохранить"
                    primary: true
                    onClicked: MortgageVM.save()
                }
            }

            GridLayout {
                Layout.fillWidth: true
                columns: page.width > UiTheme.u * 90 ? 3 : (page.width > UiTheme.u * 55 ? 2 : 1)
                rowSpacing: UiTheme.spaceMd
                columnSpacing: UiTheme.spaceMd

                LabeledField {
                    label: "Название кредита"
                    helper: "Как отображать ипотеку в отчётах"
                    text: MortgageVM.name
                    onEditingFinished: MortgageVM.name = text
                    Layout.fillWidth: true
                }
                LabeledField {
                    label: "Сумма кредита, ₽"
                    helper: "Основной долг (тело кредита)"
                    text: MortgageVM.principalRub.toFixed(0)
                    onEditingFinished: MortgageVM.principalRub = Number(text.replace(",", "."))
                    Layout.fillWidth: true
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                }
                LabeledField {
                    label: "Процентная ставка, % годовых"
                    helper: "Номинальная ставка банка"
                    text: MortgageVM.ratePercent.toFixed(2)
                    onEditingFinished: MortgageVM.ratePercent = Number(text.replace(",", "."))
                    Layout.fillWidth: true
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                }
                LabeledField {
                    label: "Срок кредита, месяцев"
                    helper: "Полный срок до погашения"
                    text: String(MortgageVM.termMonths)
                    onEditingFinished: MortgageVM.termMonths = Number(text)
                    Layout.fillWidth: true
                    inputMethodHints: Qt.ImhDigitsOnly
                }
                LabeledField {
                    label: "Досрочный платёж, ₽ / мес"
                    helper: "Дополнительно сверх аннуитета каждый месяц"
                    text: MortgageVM.extraPaymentRub.toFixed(0)
                    onEditingFinished: MortgageVM.extraPaymentRub = Number(text.replace(",", "."))
                    Layout.fillWidth: true
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                }
            }

            GridLayout {
                Layout.fillWidth: true
                columns: page.width > UiTheme.u * 80 ? 4 : 2
                rowSpacing: UiTheme.spaceMd
                columnSpacing: UiTheme.spaceMd

                FinanceCard {
                    title: "Аннуитетный платёж"
                    cents: MortgageVM.monthlyPaymentCents
                    accentColor: ThemeManager.accentColor
                    Layout.fillWidth: true
                }
                FinanceCard {
                    title: "Переплата (база)"
                    cents: MortgageVM.totalInterestCents
                    accentColor: ThemeManager.dangerColor
                    Layout.fillWidth: true
                }
                FinanceCard {
                    title: "Переплата (с досрочкой)"
                    cents: MortgageVM.totalInterestWithExtraCents
                    accentColor: ThemeManager.accentColor
                    Layout.fillWidth: true
                }
                FinanceCard {
                    title: "Экономия на %"
                    cents: MortgageVM.interestSavedCents
                    accentColor: ThemeManager.successColor
                    Layout.fillWidth: true
                }
            }

            Text {
                text: "Срок: " + MortgageVM.monthsBase + " мес → с досрочкой " + MortgageVM.monthsWithExtra + " мес"
                color: ThemeManager.mutedTextColor
                font.pixelSize: UiTheme.fontMd
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }

            // Remaining balance: base vs early repayment
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: Math.max(UiTheme.u * 28, page.height * 0.32)
                radius: UiTheme.radiusXl
                color: ThemeManager.surfaceColor
                border.color: ThemeManager.borderColor
                border.width: UiTheme.borderWidth

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: UiTheme.spaceMd
                    spacing: UiTheme.spaceSm

                    Text {
                        text: "Остаток долга по месяцам"
                        font.bold: true
                        font.pixelSize: UiTheme.fontLg
                        color: ThemeManager.textColor
                        Layout.fillWidth: true
                    }
                    Text {
                        text: "Базовый график и график с ежемесячным досрочным погашением"
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
                        seriesA: MortgageVM.remainingSeriesBase
                        seriesB: MortgageVM.remainingSeriesExtra
                        colorA: ThemeManager.accentColor
                        colorB: ThemeManager.successColor
                        sharedScale: true
                    }
                }
            }

            // Cumulative overpayment
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: Math.max(UiTheme.u * 28, page.height * 0.32)
                radius: UiTheme.radiusXl
                color: ThemeManager.surfaceColor
                border.color: ThemeManager.borderColor
                border.width: UiTheme.borderWidth

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: UiTheme.spaceMd
                    spacing: UiTheme.spaceSm

                    Text {
                        text: "Накопленная переплата (проценты)"
                        font.bold: true
                        font.pixelSize: UiTheme.fontLg
                        color: ThemeManager.textColor
                        Layout.fillWidth: true
                    }
                    Text {
                        text: "База: "
                              + (ThemeManager.isPrivacyMode ? "••••" : MoneyFormatter.formatCents(MortgageVM.totalInterestCents))
                              + "  ·  С досрочкой: "
                              + (ThemeManager.isPrivacyMode ? "••••" : MoneyFormatter.formatCents(MortgageVM.totalInterestWithExtraCents))
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
                            color: ThemeManager.dangerColor
                        }
                        Text {
                            text: "Переплата база"
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
                            text: "Переплата с досрочкой"
                            color: ThemeManager.mutedTextColor
                            font.pixelSize: UiTheme.fontSm
                        }
                    }
                    LineChart {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        seriesA: MortgageVM.interestSeriesBase
                        seriesB: MortgageVM.interestSeriesExtra
                        colorA: ThemeManager.dangerColor
                        colorB: ThemeManager.successColor
                        sharedScale: true
                    }
                }
            }

            Text {
                text: "График роста капитала — на вкладке «Капитал» (прогноз чистого капитала: счета + портфель − остаток ипотеки)."
                color: ThemeManager.mutedTextColor
                font.pixelSize: UiTheme.fontSm
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: Math.max(UiTheme.u * 36, page.height * 0.4)
                radius: UiTheme.radiusXl
                color: ThemeManager.surfaceColor
                border.color: ThemeManager.borderColor
                border.width: UiTheme.borderWidth

                ListView {
                    id: sched
                    anchors.fill: parent
                    anchors.margins: UiTheme.spaceMd
                    clip: true
                    model: MortgageVM.scheduleBase
                    header: ColumnLayout {
                        width: sched.width
                        spacing: UiTheme.spaceSm
                        Text {
                            text: "Таблица графика погашения (база)"
                            font.bold: true
                            font.pixelSize: UiTheme.fontLg
                            color: ThemeManager.textColor
                            Layout.fillWidth: true
                        }
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: UiTheme.spaceSm
                            Text {
                                text: "Месяц"
                                color: ThemeManager.mutedTextColor
                                font.pixelSize: UiTheme.fontSm
                                font.bold: true
                                Layout.preferredWidth: sched.width * 0.12
                            }
                            Text {
                                text: "Платёж"
                                color: ThemeManager.mutedTextColor
                                font.pixelSize: UiTheme.fontSm
                                font.bold: true
                                Layout.preferredWidth: sched.width * 0.22
                            }
                            Text {
                                text: "Проценты"
                                color: ThemeManager.mutedTextColor
                                font.pixelSize: UiTheme.fontSm
                                font.bold: true
                                Layout.preferredWidth: sched.width * 0.22
                            }
                            Text {
                                text: "Тело"
                                color: ThemeManager.mutedTextColor
                                font.pixelSize: UiTheme.fontSm
                                font.bold: true
                                Layout.preferredWidth: sched.width * 0.22
                            }
                            Text {
                                text: "Остаток"
                                color: ThemeManager.mutedTextColor
                                font.pixelSize: UiTheme.fontSm
                                font.bold: true
                                Layout.fillWidth: true
                            }
                        }
                        Item { Layout.preferredHeight: UiTheme.spaceXs }
                    }
                    delegate: Item {
                        id: schedRow
                        width: sched.width
                        height: UiTheme.controlHeight * 0.75
                        required property int monthIndex
                        required property real paymentCents
                        required property real interestCents
                        required property real principalCents
                        required property real remainingCents
                        RowLayout {
                            anchors.fill: parent
                            spacing: UiTheme.spaceSm
                            Text {
                                text: "#" + schedRow.monthIndex
                                color: ThemeManager.mutedTextColor
                                font.pixelSize: UiTheme.fontSm
                                Layout.preferredWidth: sched.width * 0.12
                            }
                            PrivacyAmount {
                                cents: schedRow.paymentCents
                                font.pixelSize: UiTheme.fontSm
                                Layout.preferredWidth: sched.width * 0.22
                            }
                            PrivacyAmount {
                                cents: schedRow.interestCents
                                font.pixelSize: UiTheme.fontSm
                                Layout.preferredWidth: sched.width * 0.22
                            }
                            PrivacyAmount {
                                cents: schedRow.principalCents
                                font.pixelSize: UiTheme.fontSm
                                Layout.preferredWidth: sched.width * 0.22
                            }
                            PrivacyAmount {
                                cents: schedRow.remainingCents
                                font.pixelSize: UiTheme.fontSm
                                Layout.fillWidth: true
                            }
                        }
                    }
                }
            }
        }
    }
}
