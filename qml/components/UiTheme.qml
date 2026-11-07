pragma Singleton
import QtQuick

QtObject {
    id: root

    // Bind from ApplicationWindow / page root via setHost(item)
    property real hostWidth: 1200
    property real hostHeight: 780

    readonly property real u: Math.max(6, Math.min(hostWidth, hostHeight) / 100)
    readonly property real spaceXs: u * 0.6
    readonly property real spaceSm: u * 1.0
    readonly property real spaceMd: u * 1.6
    readonly property real spaceLg: u * 2.4
    readonly property real spaceXl: u * 3.2

    // Soft rounded UI (example cards/pills), scaled to window — keep palette elsewhere
    readonly property real radiusSm: u * 1.4
    readonly property real radiusMd: u * 2.2
    readonly property real radiusLg: u * 3.2
    readonly property real radiusXl: u * 4.0
    readonly property real radiusPill: 999

    readonly property real fontSm: Math.round(u * 1.55)
    readonly property real fontMd: Math.round(u * 1.95)
    readonly property real fontLg: Math.round(u * 2.45)
    readonly property real fontXl: Math.round(u * 3.3)
    readonly property real fontTitle: Math.round(u * 3.7)

    readonly property real controlHeight: u * 4.6
    readonly property real buttonMinWidth: u * 13
    readonly property real navWidth: Math.max(u * 24, hostWidth * 0.17)
    readonly property real cardMinHeight: u * 10
    readonly property real pagePadding: spaceXl
    readonly property real progressHeight: Math.max(8, u * 1.1)
    readonly property real borderWidth: Math.max(1, Math.round(u * 0.16))
    readonly property real iconSm: u * 2.8
    readonly property real iconMd: u * 4.2
    readonly property real iconLg: u * 6.5

    function setHost(item) {
        if (!item)
            return
        hostWidth = item.width
        hostHeight = item.height
    }
}
