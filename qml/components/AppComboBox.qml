import QtQuick
import QtQuick.Controls
import FamilyBudget 1.0
import components 1.0

ComboBox {
    id: root

    font.pixelSize: UiTheme.fontMd
    implicitHeight: UiTheme.controlHeight
    leftPadding: UiTheme.spaceMd
    rightPadding: UiTheme.spaceMd + indicatorItem.width
    topPadding: 0
    bottomPadding: 0
    spacing: UiTheme.spaceSm

    readonly property real _itemH: UiTheme.controlHeight
    readonly property int _count: {
        if (!model)
            return 0
        if (typeof model === "number")
            return 0
        if (model.count !== undefined)
            return model.count
        if (model.length !== undefined)
            return model.length
        if (typeof count === "number")
            return count
        return 0
    }

    contentItem: Text {
        text: root.displayText
        font: root.font
        color: ThemeManager.textColor
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        leftPadding: 0
        rightPadding: UiTheme.spaceSm
    }

    background: Rectangle {
        radius: UiTheme.radiusLg
        color: ThemeManager.isDarkMode ? "#0B1220" : "#F8FAFC"
        border.width: UiTheme.borderWidth
        border.color: root.activeFocus || root.down ? ThemeManager.accentColor
                                                    : ThemeManager.borderColor
        Behavior on border.color { ColorAnimation { duration: 120 } }
    }

    indicator: Item {
        id: indicatorItem
        implicitWidth: Math.max(UiTheme.iconSm, UiTheme.fontMd * 1.4)
        implicitHeight: root.height
        x: root.width - width - UiTheme.spaceSm
        y: 0

        Text {
            anchors.centerIn: parent
            text: root.down ? "▴" : "▾"
            color: ThemeManager.mutedTextColor
            font.pixelSize: UiTheme.fontMd
            font.bold: true
        }
    }

    popup: Popup {
        id: comboPopup
        padding: Math.max(4, UiTheme.spaceXs)
        width: root.width
        // Exact height from item count — avoids empty “ghost” popup
        height: {
            var n = Math.max(root._count, 1)
            var content = n * root._itemH
            var maxH = UiTheme.u * 40
            return Math.min(content, maxH) + topPadding + bottomPadding
        }
        modal: false
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
        // Prefer local parent; Overlay only when needed (dialogs)
        parent: root

        background: Rectangle {
            radius: UiTheme.radiusLg
            color: ThemeManager.surfaceColor
            border.width: UiTheme.borderWidth
            border.color: ThemeManager.borderColor
        }

        contentItem: ListView {
            id: list
            clip: true
            implicitHeight: contentHeight
            model: root.delegateModel
            currentIndex: root.highlightedIndex
            boundsBehavior: Flickable.StopAtBounds
            interactive: contentHeight > height
            ScrollBar.vertical: ScrollBar {
                policy: list.contentHeight > list.height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
                width: Math.max(4, UiTheme.u * 0.55)
            }
        }

        function insideDialog() {
            var p = root.parent
            while (p) {
                // Dialog / Popup ancestors clip children — use overlay there
                if (p instanceof Popup)
                    return true
                p = p.parent
            }
            return false
        }

        function reposition() {
            var gap = Math.max(4, UiTheme.spaceXs)
            width = root.width
            if (insideDialog() && Overlay.overlay) {
                parent = Overlay.overlay
                var pt = root.mapToItem(Overlay.overlay, 0, root.height + gap)
                x = pt.x
                y = pt.y
                // Flip up if not enough space below
                if (y + height > Overlay.overlay.height - UiTheme.spaceMd) {
                    var up = root.mapToItem(Overlay.overlay, 0, -height - gap)
                    if (up.y > UiTheme.spaceMd)
                        y = up.y
                }
            } else {
                parent = root
                x = 0
                y = root.height + gap
            }
        }

        onAboutToShow: reposition()
        onOpened: Qt.callLater(reposition)
        onClosed: parent = root
    }

    delegate: ItemDelegate {
        id: comboDelegate
        required property int index

        width: ListView.view ? ListView.view.width : root.width
        height: root._itemH
        padding: 0
        leftPadding: UiTheme.spaceMd
        rightPadding: UiTheme.spaceMd
        highlighted: root.highlightedIndex === comboDelegate.index

        contentItem: Text {
            text: root.textRole && (typeof model !== "undefined") && model[root.textRole] !== undefined
                  ? model[root.textRole]
                  : ((typeof modelData !== "undefined" && modelData !== null) ? ("" + modelData) : "")
            color: ThemeManager.textColor
            font: root.font
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
            width: comboDelegate.availableWidth
        }

        background: Rectangle {
            anchors.fill: parent
            anchors.leftMargin: Math.max(1, UiTheme.u * 0.2)
            anchors.rightMargin: Math.max(1, UiTheme.u * 0.2)
            anchors.topMargin: Math.max(1, UiTheme.u * 0.1)
            anchors.bottomMargin: Math.max(1, UiTheme.u * 0.1)
            radius: UiTheme.radiusMd
            color: comboDelegate.highlighted || comboDelegate.hovered
                   ? ThemeManager.selectionBgColor
                   : "transparent"
            border.width: (comboDelegate.highlighted || comboDelegate.hovered) ? UiTheme.borderWidth : 0
            border.color: ThemeManager.selectionBorderColor
        }
    }
}
