import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import FamilyBudget 1.0
import components 1.0
import views 1.0

ApplicationWindow {
    id: window
    width: Screen.width * 0.72
    height: Screen.height * 0.78
    minimumWidth: Math.max(360, Screen.width * 0.28)
    minimumHeight: Math.max(560, Screen.height * 0.45)
    visible: false
    title: "Семейный Бюджет"
    color: ThemeManager.backgroundColor

    Behavior on color { ColorAnimation { duration: 400; easing.type: Easing.InOutQuad } }

    readonly property bool narrow: width < UiTheme.u * 90
    property int navIndex: 0

    function syncThemeHost() {
        UiTheme.hostWidth = width
        UiTheme.hostHeight = height
    }

    onWidthChanged: syncThemeHost()
    onHeightChanged: syncThemeHost()
    Component.onCompleted: syncThemeHost()

    function navigateTo(index) {
        if (typeof index !== "number" || index < 0)
            return
        if (window.navIndex !== index)
            window.navIndex = index
        const page = pageFor(index)
        if (window.narrow)
            stackMobile.replace(null, page)
        else
            stack.replace(null, page)
    }

    function showToast(ok, message) {
        toast.text = message
        toast.color = ok ? ThemeManager.successColor : ThemeManager.dangerColor
        toastOpacity.restart()
    }

    Connections {
        target: TransactionVM
        function onOperationFinished(ok, message) { window.showToast(ok, message) }
    }
    Connections {
        target: GoalVM
        function onOperationFinished(ok, message) { window.showToast(ok, message) }
    }
    Connections {
        target: InvestmentVM
        function onOperationFinished(ok, message) { window.showToast(ok, message) }
    }
    Connections {
        target: MortgageVM
        function onOperationFinished(ok, message) { window.showToast(ok, message) }
    }
    Connections {
        target: SettingsVM
        function onOperationFinished(ok, message) { window.showToast(ok, message) }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0
        visible: !window.narrow

        NavRail {
            id: rail
            Layout.fillHeight: true
            Layout.preferredWidth: UiTheme.navWidth
            Layout.maximumWidth: window.width * 0.28
            currentIndex: window.navIndex
        }
        Connections {
            target: rail
            function onNavigated(index) { window.navigateTo(index) }
        }

        StackView {
            id: stack
            Layout.fillWidth: true
            Layout.fillHeight: true
            initialItem: dashboardComp
            pushEnter: Transition {
                NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 350; easing.type: Easing.InOutQuint }
                NumberAnimation { property: "x"; from: width * 0.04; to: 0; duration: 350; easing.type: Easing.InOutQuint }
            }
            pushExit: Transition {
                NumberAnimation { property: "opacity"; from: 1; to: 0; duration: 250 }
            }
            replaceEnter: pushEnter
            replaceExit: pushExit
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        visible: window.narrow

        StackView {
            id: stackMobile
            Layout.fillWidth: true
            Layout.fillHeight: true
            initialItem: dashboardComp
        }

        TabBar {
            id: tabBar
            Layout.fillWidth: true
            Layout.preferredHeight: UiTheme.controlHeight * 1.15
            visible: !SettingsStore.hideChromeForRecording
            background: Rectangle {
                color: ThemeManager.surfaceColor
                radius: UiTheme.radiusLg
            }
            component SoftTab: TabButton {
                font.pixelSize: UiTheme.fontLg
                background: Rectangle {
                    radius: Math.min(height / 2, UiTheme.radiusLg)
                    color: parent.checked ? ThemeManager.selectionBgColor
                                          : (parent.hovered ? ThemeManager.borderColor : "transparent")
                }
            }
            SoftTab { text: "⌂" }
            SoftTab { text: "⇄" }
            SoftTab { text: "◎" }
            SoftTab { text: "↗" }
            SoftTab { text: "И" }
            SoftTab { text: "◈" }
            SoftTab { text: "⚙" }
            currentIndex: window.navIndex
            onCurrentIndexChanged: {
                if (typeof currentIndex === "number" && currentIndex !== window.navIndex)
                    window.navigateTo(currentIndex)
            }
        }
    }

    Text {
        id: toast
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: UiTheme.spaceLg
        z: 100
        opacity: 0
        font.pixelSize: UiTheme.fontMd
        font.bold: true
        padding: UiTheme.spaceSm
    }
    SequentialAnimation {
        id: toastOpacity
        NumberAnimation { target: toast; property: "opacity"; to: 1; duration: 120 }
        PauseAnimation { duration: 1800 }
        NumberAnimation { target: toast; property: "opacity"; to: 0; duration: 250 }
    }

    Component { id: dashboardComp; DashboardView {} }
    Component { id: txComp; TransactionsView {} }
    Component { id: goalsComp; GoalsView {} }
    Component { id: invComp; InvestmentsView {} }
    Component { id: mortComp; MortgageView {} }
    Component { id: capComp; CapitalView {} }
    Component { id: setComp; SettingsView {} }

    function pageFor(index) {
        switch (index) {
        case 0: return dashboardComp
        case 1: return txComp
        case 2: return goalsComp
        case 3: return invComp
        case 4: return mortComp
        case 5: return capComp
        case 6: return setComp
        }
        return dashboardComp
    }
}
