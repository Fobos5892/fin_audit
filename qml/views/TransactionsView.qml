import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FamilyBudget 1.0
import components 1.0

Item {
    id: page

    function resetForm(amountField, descField, ownershipBox) {
        amountField.text = ""
        descField.text = ""
        ownershipBox.currentIndex = 0
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: UiTheme.pagePadding
        spacing: UiTheme.spaceMd

        RowLayout {
            Layout.fillWidth: true
            spacing: UiTheme.spaceSm
            Text {
                text: "Операции"
                font.pixelSize: UiTheme.fontTitle
                font.bold: true
                color: ThemeManager.textColor
                Layout.fillWidth: true
            }
            ColumnLayout {
                spacing: UiTheme.spaceXs
                Text {
                    text: "Чьи операции"
                    color: ThemeManager.mutedTextColor
                    font.pixelSize: UiTheme.fontSm
                    font.bold: true
                }
                AppComboBox {
                    id: ownershipFilter
                    model: ["Все", "Семейные", "Личные"]
                    Layout.preferredWidth: Math.max(UiTheme.u * 18, page.width * 0.18)
                    Layout.preferredHeight: UiTheme.controlHeight
                    onActivated: function(i) {
                        if (i === 0) TransactionVM.filterOwnership = ""
                        else if (i === 1) TransactionVM.filterOwnership = "family"
                        else TransactionVM.filterOwnership = "personal"
                    }
                }
                Text {
                    text: "Семейные — общий бюджет · Личные — ваши"
                    color: ThemeManager.mutedTextColor
                    font.pixelSize: Math.round(UiTheme.fontSm * 0.9)
                    Layout.preferredWidth: ownershipFilter.Layout.preferredWidth
                    wrapMode: Text.WordWrap
                }
            }
            AppButton {
                text: "Добавить доход"
                primary: true
                Layout.alignment: Qt.AlignBottom
                onClicked: incomeDialog.open()
            }
            AppButton {
                text: "Добавить расход"
                danger: true
                Layout.alignment: Qt.AlignBottom
                onClicked: expenseDialog.open()
            }
        }

        GridLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            columns: page.width > UiTheme.u * 75 ? 2 : 1
            rowSpacing: UiTheme.spaceMd
            columnSpacing: UiTheme.spaceMd

            TransactionMiniList {
                Layout.fillWidth: true
                Layout.fillHeight: true
                title: "Доходы"
                model: TransactionVM.incomeTransactions
                emptyText: "Нет доходов"
                accent: ThemeManager.successColor
                showRemove: true
                onRemoveRequested: function(id) { TransactionVM.removeTransaction(id) }
            }
            TransactionMiniList {
                Layout.fillWidth: true
                Layout.fillHeight: true
                title: "Расходы"
                model: TransactionVM.expenseTransactions
                emptyText: "Нет расходов"
                accent: ThemeManager.dangerColor
                showRemove: true
                onRemoveRequested: function(id) { TransactionVM.removeTransaction(id) }
            }
        }
    }

    // —— Доход ——
    AppDialog {
        id: incomeDialog
        title: "Добавить доход"
        width: Math.min(page.width * 0.55, UiTheme.u * 55)
        acceptText: "Сохранить"
        rejectText: "Отмена"

        ColumnLayout {
            width: incomeDialog.availableWidth
            spacing: UiTheme.spaceMd

            Text {
                text: "Счёт зачисления"
                color: ThemeManager.mutedTextColor
                font.pixelSize: UiTheme.fontSm
                font.bold: true
            }
            AppComboBox {
                id: incomeAccount
                Layout.fillWidth: true
                Layout.preferredHeight: UiTheme.controlHeight
                textRole: "name"
                model: TransactionVM.accounts
            }
            Text {
                text: "Категория дохода"
                color: ThemeManager.mutedTextColor
                font.pixelSize: UiTheme.fontSm
                font.bold: true
            }
            AppComboBox {
                id: incomeCategory
                Layout.fillWidth: true
                Layout.preferredHeight: UiTheme.controlHeight
                textRole: "name"
                model: TransactionVM.incomeCategories
            }
            Text {
                visible: TransactionVM.incomeCategories.count === 0
                text: "Нет категорий дохода — добавьте их в Настройках."
                color: ThemeManager.dangerColor
                font.pixelSize: UiTheme.fontSm
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }
            LabeledField {
                id: incomeAmount
                label: "Сумма, ₽"
                placeholderText: "0"
                helper: "Сколько поступило"
                Layout.fillWidth: true
                inputMethodHints: Qt.ImhFormattedNumbersOnly
            }
            LabeledField {
                id: incomeDesc
                label: "Описание"
                placeholderText: "Например, аванс за март"
                Layout.fillWidth: true
            }
            Text {
                text: "Чьи это деньги"
                color: ThemeManager.mutedTextColor
                font.pixelSize: UiTheme.fontSm
                font.bold: true
            }
            AppComboBox {
                id: incomeOwnership
                model: ["Семейное", "Личное"]
                Layout.fillWidth: true
                Layout.preferredHeight: UiTheme.controlHeight
            }
            Text {
                text: "Семейное — в общий бюджет. Личное — только ваши."
                color: ThemeManager.mutedTextColor
                font.pixelSize: Math.round(UiTheme.fontSm * 0.92)
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }
        }

        onAccepted: {
            TransactionVM.submitTransaction(
                incomeAccount.currentIndex, incomeCategory.currentIndex,
                incomeAmount.text, incomeDesc.text,
                MoneyFormatter.ownershipCodeFromIndex(incomeOwnership.currentIndex),
                true)
            page.resetForm(incomeAmount, incomeDesc, incomeOwnership)
        }
    }

    // —— Расход ——
    AppDialog {
        id: expenseDialog
        title: "Добавить расход"
        width: Math.min(page.width * 0.55, UiTheme.u * 55)
        acceptText: "Сохранить"
        rejectText: "Отмена"

        ColumnLayout {
            width: expenseDialog.availableWidth
            spacing: UiTheme.spaceMd

            Text {
                text: "Счёт списания"
                color: ThemeManager.mutedTextColor
                font.pixelSize: UiTheme.fontSm
                font.bold: true
            }
            AppComboBox {
                id: expenseAccount
                Layout.fillWidth: true
                Layout.preferredHeight: UiTheme.controlHeight
                textRole: "name"
                model: TransactionVM.accounts
            }
            Text {
                text: "Категория расхода"
                color: ThemeManager.mutedTextColor
                font.pixelSize: UiTheme.fontSm
                font.bold: true
            }
            AppComboBox {
                id: expenseCategory
                Layout.fillWidth: true
                Layout.preferredHeight: UiTheme.controlHeight
                textRole: "name"
                model: TransactionVM.expenseCategories
            }
            Text {
                visible: TransactionVM.expenseCategories.count === 0
                text: "Нет категорий расхода — добавьте их в Настройках."
                color: ThemeManager.dangerColor
                font.pixelSize: UiTheme.fontSm
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }
            LabeledField {
                id: expenseAmount
                label: "Сумма, ₽"
                placeholderText: "0"
                helper: "Сколько потратили (положительное число)"
                Layout.fillWidth: true
                inputMethodHints: Qt.ImhFormattedNumbersOnly
            }
            LabeledField {
                id: expenseDesc
                label: "Описание"
                placeholderText: "Например, продукты в Пятёрочке"
                Layout.fillWidth: true
            }
            Text {
                text: "Чьи это деньги"
                color: ThemeManager.mutedTextColor
                font.pixelSize: UiTheme.fontSm
                font.bold: true
            }
            AppComboBox {
                id: expenseOwnership
                model: ["Семейное", "Личное"]
                Layout.fillWidth: true
                Layout.preferredHeight: UiTheme.controlHeight
            }
            Text {
                text: "Семейное — из общего бюджета. Личное — ваши отдельные траты."
                color: ThemeManager.mutedTextColor
                font.pixelSize: Math.round(UiTheme.fontSm * 0.92)
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }
        }

        onAccepted: {
            TransactionVM.submitTransaction(
                expenseAccount.currentIndex, expenseCategory.currentIndex,
                expenseAmount.text, expenseDesc.text,
                MoneyFormatter.ownershipCodeFromIndex(expenseOwnership.currentIndex),
                false)
            page.resetForm(expenseAmount, expenseDesc, expenseOwnership)
        }
    }
}
