import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import FamilyBudget 1.0
import components 1.0

Item {
    id: page
    property int editingCategoryId: -1

    function colorToHex(c) {
        function h(n) {
            var s = Math.round(n * 255).toString(16)
            return s.length < 2 ? "0" + s : s
        }
        return "#" + h(c.r) + h(c.g) + h(c.b)
    }

    function typeLabel(t) {
        return t === "income" ? "доход" : "расход"
    }

    ScrollView {
        id: scroller
        anchors.fill: parent
        anchors.margins: UiTheme.pagePadding
        clip: true
        contentWidth: availableWidth

        ColumnLayout {
            width: scroller.availableWidth
            spacing: UiTheme.spaceLg

            Text {
                text: "Настройки"
                font.pixelSize: UiTheme.fontTitle
                font.bold: true
                color: ThemeManager.textColor
            }

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: switches.implicitHeight + UiTheme.spaceLg * 2
                radius: UiTheme.radiusXl
                color: ThemeManager.surfaceColor
                border.color: ThemeManager.borderColor
                border.width: UiTheme.borderWidth

                ColumnLayout {
                    id: switches
                    anchors.fill: parent
                    anchors.margins: UiTheme.spaceMd
                    spacing: UiTheme.spaceSm
                    Switch {
                        text: "Тёмная тема"
                        checked: SettingsVM.isDarkMode
                        onToggled: SettingsVM.isDarkMode = checked
                        contentItem: Text {
                            text: parent.text
                            color: ThemeManager.textColor
                            font.pixelSize: UiTheme.fontMd
                            leftPadding: parent.indicator.width + UiTheme.spaceSm
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                    Switch {
                        text: "Privacy Mode (скрытие сумм)"
                        checked: SettingsVM.isPrivacyMode
                        onToggled: SettingsVM.isPrivacyMode = checked
                        contentItem: Text {
                            text: parent.text
                            color: ThemeManager.textColor
                            font.pixelSize: UiTheme.fontMd
                            leftPadding: parent.indicator.width + UiTheme.spaceSm
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                    Switch {
                        text: "Скрыть служебные кнопки при записи экрана"
                        checked: SettingsVM.hideChromeForRecording
                        onToggled: SettingsVM.hideChromeForRecording = checked
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
            }

            Text {
                text: "Категории"
                font.bold: true
                font.pixelSize: UiTheme.fontLg
                color: ThemeManager.textColor
            }
            Text {
                text: "Отдельно для доходов и расходов. Их видно в формах добавления операций и на графиках."
                color: ThemeManager.mutedTextColor
                font.pixelSize: UiTheme.fontSm
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }

            GridLayout {
                Layout.fillWidth: true
                columns: page.width > UiTheme.u * 75 ? 2 : 1
                rowSpacing: UiTheme.spaceMd
                columnSpacing: UiTheme.spaceMd

                // —— Доходы ——
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: Math.max(UiTheme.u * 42, page.height * 0.45)
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
                            Text {
                                text: "Категории доходов"
                                font.bold: true
                                font.pixelSize: UiTheme.fontLg
                                color: ThemeManager.textColor
                                Layout.fillWidth: true
                            }
                            Text {
                                text: SettingsVM.incomeCategories.count
                                color: ThemeManager.mutedTextColor
                                font.pixelSize: UiTheme.fontSm
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: UiTheme.spaceSm
                            LabeledField {
                                id: newIncomeName
                                label: ""
                                placeholderText: "Новая категория дохода"
                                Layout.fillWidth: true
                                compact: true
                            }
                            AppButton {
                                text: "Добавить"
                                primary: true
                                Layout.alignment: Qt.AlignBottom
                                onClicked: {
                                    SettingsVM.addCategory(newIncomeName.text, true)
                                    newIncomeName.text = ""
                                }
                            }
                        }

                        ListView {
                            id: incomeCatList
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            clip: true
                            spacing: UiTheme.spaceSm
                            model: SettingsVM.incomeCategories
                            boundsBehavior: Flickable.StopAtBounds
                            delegate: categoryDelegate
                        }
                    }
                }

                // —— Расходы ——
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: Math.max(UiTheme.u * 42, page.height * 0.45)
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
                            Text {
                                text: "Категории расходов"
                                font.bold: true
                                font.pixelSize: UiTheme.fontLg
                                color: ThemeManager.textColor
                                Layout.fillWidth: true
                            }
                            Text {
                                text: SettingsVM.expenseCategories.count
                                color: ThemeManager.mutedTextColor
                                font.pixelSize: UiTheme.fontSm
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: UiTheme.spaceSm
                            LabeledField {
                                id: newExpenseName
                                label: ""
                                placeholderText: "Новая категория расхода"
                                Layout.fillWidth: true
                                compact: true
                            }
                            AppButton {
                                text: "Добавить"
                                primary: true
                                Layout.alignment: Qt.AlignBottom
                                onClicked: {
                                    SettingsVM.addCategory(newExpenseName.text, false)
                                    newExpenseName.text = ""
                                }
                            }
                        }

                        ListView {
                            id: expenseCatList
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            clip: true
                            spacing: UiTheme.spaceSm
                            model: SettingsVM.expenseCategories
                            boundsBehavior: Flickable.StopAtBounds
                            delegate: categoryDelegate
                        }
                    }
                }
            }
        }
    }

    Component {
        id: categoryDelegate
        Item {
            id: catRow
            width: ListView.view ? ListView.view.width : parent.width
            height: Math.max(UiTheme.controlHeight * 1.2, row.implicitHeight + UiTheme.spaceSm)
            required property int id
            required property string name
            required property string type
            required property string chartColor

            Rectangle {
                anchors.fill: parent
                radius: UiTheme.radiusLg
                color: ThemeManager.isDarkMode ? "#243044" : "#F1F5F9"
                RowLayout {
                    id: row
                    anchors.fill: parent
                    anchors.margins: UiTheme.spaceSm
                    spacing: UiTheme.spaceSm
                    Rectangle {
                        Layout.preferredWidth: UiTheme.iconSm * 0.7
                        Layout.preferredHeight: Layout.preferredWidth
                        radius: width / 2
                        color: catRow.chartColor
                    }
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2
                        Text {
                            text: catRow.name
                            color: ThemeManager.textColor
                            font.pixelSize: UiTheme.fontMd
                            font.bold: true
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }
                        Text {
                            text: page.typeLabel(catRow.type)
                            color: ThemeManager.mutedTextColor
                            font.pixelSize: UiTheme.fontSm
                        }
                    }
                    AppButton {
                        text: "Цвет"
                        onClicked: {
                            editingCategoryId = catRow.id
                            colorDialog.selectedColor = catRow.chartColor
                            colorDialog.open()
                        }
                    }
                    AppButton {
                        text: "✕"
                        danger: true
                        Layout.preferredWidth: UiTheme.controlHeight
                        leftPadding: 0
                        rightPadding: 0
                        onClicked: SettingsVM.removeCategory(catRow.id)
                    }
                }
            }
        }
    }

    ColorDialog {
        id: colorDialog
        title: "Цвет категории"
        onAccepted: {
            if (editingCategoryId > 0)
                SettingsVM.updateCategoryColor(editingCategoryId, colorToHex(selectedColor))
        }
    }
}
