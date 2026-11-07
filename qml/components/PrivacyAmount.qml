import QtQuick
import FamilyBudget 1.0
import components 1.0

Text {
    id: root
    property real cents: 0
    property string currency: "₽"
    property bool showSign: false
    property color valueColor: ThemeManager.textColor

    color: valueColor
    font.pixelSize: UiTheme.fontXl
    font.bold: true

    text: ThemeManager.isPrivacyMode
          ? ("•••• " + currency)
          : (showSign
             ? MoneyFormatter.formatCentsSigned(cents, currency)
             : MoneyFormatter.formatCents(cents, currency))
}
