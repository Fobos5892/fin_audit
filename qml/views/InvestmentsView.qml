import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FamilyBudget 1.0
import components 1.0

Item {
    id: page

    readonly property real pagePad: UiTheme.spaceXl

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: page.pagePad
        spacing: UiTheme.spaceLg

        RowLayout {
            Layout.fillWidth: true
            spacing: UiTheme.spaceMd

            Text {
                text: "Инвестиции"
                font.pixelSize: UiTheme.fontTitle
                font.bold: true
                color: ThemeManager.textColor
                Layout.fillWidth: true
                elide: Text.ElideRight
            }

            AppButton {
                text: "Сделка"
                primary: true
                onClicked: tradeDialog.open()
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: UiTheme.spaceMd

            FinanceCard {
                title: "Портфель"
                cents: InvestmentVM.portfolioValueCents
                accentColor: ThemeManager.accentColor
                Layout.fillWidth: true
                Layout.preferredHeight: implicitHeight
            }
            FinanceCard {
                title: "P&L"
                cents: InvestmentVM.pnlCents
                accentColor: InvestmentVM.pnlCents >= 0 ? ThemeManager.successColor : ThemeManager.dangerColor
                Layout.fillWidth: true
                Layout.preferredHeight: implicitHeight
            }
        }

        Text {
            text: "Доходность: " + InvestmentVM.pnlPercent.toFixed(2) + "%"
            color: ThemeManager.mutedTextColor
            font.pixelSize: UiTheme.fontMd
            Layout.topMargin: UiTheme.spaceXs
            Layout.bottomMargin: UiTheme.spaceXs
        }

        ListView {
            id: list
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: UiTheme.spaceMd
            model: InvestmentVM.positions
            delegate: Item {
                id: invRow
                width: list.width
                height: card.implicitHeight
                required property int id
                required property string ticker
                required property string name
                required property string type
                required property real quantity
                required property real valueCents
                required property real pnlCents

                Rectangle {
                    id: card
                    width: parent.width
                    implicitHeight: row.implicitHeight + UiTheme.spaceLg * 2
                    radius: UiTheme.radiusXl
                    color: ThemeManager.surfaceColor
                    border.color: ThemeManager.borderColor
                    border.width: UiTheme.borderWidth

                    RowLayout {
                        id: row
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.margins: UiTheme.pagePadding
                        spacing: UiTheme.spaceMd

                        ColumnLayout {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignVCenter
                            spacing: UiTheme.spaceXs

                            Text {
                                text: invRow.ticker + " — " + invRow.name
                                color: ThemeManager.textColor
                                font.bold: true
                                font.pixelSize: UiTheme.fontLg
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }
                            Text {
                                text: invRow.type + " · qty " + invRow.quantity
                                color: ThemeManager.mutedTextColor
                                font.pixelSize: UiTheme.fontSm
                            }
                        }

                        ColumnLayout {
                            Layout.alignment: Qt.AlignVCenter
                            spacing: UiTheme.spaceXs
                            Layout.rightMargin: UiTheme.spaceSm

                            PrivacyAmount {
                                cents: invRow.valueCents
                                font.pixelSize: UiTheme.fontLg
                                horizontalAlignment: Text.AlignRight
                                Layout.alignment: Qt.AlignRight
                            }
                            PrivacyAmount {
                                cents: invRow.pnlCents
                                showSign: true
                                font.pixelSize: UiTheme.fontMd
                                valueColor: invRow.pnlCents >= 0 ? ThemeManager.successColor : ThemeManager.dangerColor
                                horizontalAlignment: Text.AlignRight
                                Layout.alignment: Qt.AlignRight
                            }
                        }

                        LabeledField {
                            id: priceField
                            label: "Цена, ₽"
                            placeholderText: "Новая цена"
                            compact: true
                            Layout.preferredWidth: Math.max(UiTheme.u * 16, list.width * 0.18)
                            Layout.maximumWidth: list.width * 0.28
                            Layout.fillWidth: false
                            Layout.alignment: Qt.AlignVCenter
                            inputMethodHints: Qt.ImhFormattedNumbersOnly
                        }

                        AppButton {
                            text: "Обновить"
                            Layout.alignment: Qt.AlignVCenter
                            Layout.leftMargin: UiTheme.spaceXs
                            onClicked: InvestmentVM.updatePrice(invRow.id, priceField.text)
                        }
                    }
                }
            }
        }
    }

    AppDialog {
        id: tradeDialog
        title: "Покупка / продажа"
        width: Math.min(page.width * 0.55, UiTheme.u * 55)
        ColumnLayout {
            width: tradeDialog.availableWidth
            spacing: UiTheme.spaceMd
            LabeledField { id: ticker; label: "Тикер"; placeholderText: "Например, SBER"; Layout.fillWidth: true }
            LabeledField { id: name; label: "Название инструмента"; placeholderText: "Полное имя"; Layout.fillWidth: true }
            Text {
                text: "Тип актива"
                color: ThemeManager.mutedTextColor
                font.pixelSize: UiTheme.fontSm
                font.bold: true
            }
            AppComboBox {
                id: typeBox
                model: ["stock", "bond", "crypto", "fiat", "metal"]
                Layout.fillWidth: true
            }
            Text {
                text: "Сторона сделки"
                color: ThemeManager.mutedTextColor
                font.pixelSize: UiTheme.fontSm
                font.bold: true
            }
            AppComboBox {
                id: sideBox
                model: ["buy", "sell"]
                Layout.fillWidth: true
            }
            LabeledField {
                id: qty
                label: "Количество"
                placeholderText: "Лоты / штуки"
                Layout.fillWidth: true
                inputMethodHints: Qt.ImhFormattedNumbersOnly
            }
            LabeledField {
                id: price
                label: "Цена за единицу, ₽"
                placeholderText: "0"
                Layout.fillWidth: true
                inputMethodHints: Qt.ImhFormattedNumbersOnly
            }
        }
        onAccepted: InvestmentVM.trade(ticker.text, name.text, typeBox.currentText,
                                       sideBox.currentText === "buy", qty.text, price.text)
    }
}
