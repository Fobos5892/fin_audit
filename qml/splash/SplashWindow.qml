import QtQuick
import QtQuick.Window

Window {
    id: splash
    width: 460
    height: 560
    minimumWidth: 460
    minimumHeight: 560
    maximumWidth: 460
    maximumHeight: 560
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    color: "transparent"
    title: "Семейный Бюджет"
    visible: true

    // Soft entrance
    opacity: 0
    NumberAnimation on opacity {
        from: 0; to: 1
        duration: 420
        easing.type: Easing.OutCubic
        running: true
    }

    property real ringPhase: 0
    property real pulse: 0
    property bool holdingForLoad: SplashCtrl ? !SplashCtrl.loadComplete : true

    NumberAnimation on ringPhase {
        from: 0; to: Math.PI * 2
        duration: 5200
        loops: Animation.Infinite
        running: true
        easing.type: Easing.Linear
    }

    SequentialAnimation on pulse {
        loops: Animation.Infinite
        running: true
        NumberAnimation { from: 0.0; to: 1.0; duration: 1100; easing.type: Easing.InOutSine }
        NumberAnimation { from: 1.0; to: 0.0; duration: 1100; easing.type: Easing.InOutSine }
    }

    // Minimum cinematic beat (~1.4s), then allow dismiss when load done
    Timer {
        id: minAnimTimer
        interval: 1400
        running: true
        onTriggered: SplashCtrl.markMinAnimationDone()
    }

    // If load is slow, crawl the bar so it never feels frozen
    Timer {
        interval: 90
        running: splash.holdingForLoad
        repeat: true
        onTriggered: {
            if (!SplashCtrl || SplashCtrl.loadComplete)
                return
            var softCap = 0.42
            if (SplashCtrl.dbReady)
                softCap = 0.78
            if (SplashCtrl.uiReady)
                softCap = 0.97
            if (SplashCtrl.progress < softCap)
                SplashCtrl.bumpProgress(SplashCtrl.progress + 0.01)
        }
    }

    Connections {
        target: SplashCtrl
        function onCanDismissChanged() {
            if (SplashCtrl.canDismiss && !exitAnim.running)
                exitAnim.start()
        }
    }

    ParallelAnimation {
        id: exitAnim
        NumberAnimation { target: splash; property: "opacity"; to: 0; duration: 380; easing.type: Easing.InCubic }
        NumberAnimation { target: card; property: "scale"; to: 1.04; duration: 380; easing.type: Easing.InCubic }
        onFinished: SplashCtrl.notifySplashFinished()
    }

    Rectangle {
        id: card
        anchors.fill: parent
        radius: 28
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#07111F" }
            GradientStop { position: 0.55; color: "#0B1A2E" }
            GradientStop { position: 1.0; color: "#10253F" }
        }
        border.color: Qt.rgba(0.45, 0.72, 1.0, 0.18)
        border.width: 1
        scale: 1.0
        clip: true

        // Ambient glow blobs
        Rectangle {
            width: 280; height: 280; radius: 140
            x: -80; y: -60
            color: Qt.rgba(0.16, 0.55, 0.95, 0.16 + splash.pulse * 0.08)
        }
        Rectangle {
            width: 220; height: 220; radius: 110
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: -40
            color: Qt.rgba(0.15, 0.85, 0.65, 0.10 + splash.pulse * 0.06)
        }

        // Starfield
        Repeater {
            model: 28
            delegate: Rectangle {
                required property int index
                width: 2 + (index % 3)
                height: width
                radius: width / 2
                color: Qt.rgba(0.75, 0.88, 1.0, 0.15 + (index % 5) * 0.08)
                x: (index * 73) % (card.width - 20) + 10
                y: (index * 97) % (card.height - 20) + 10
                opacity: 0.35 + 0.45 * Math.abs(Math.sin(splash.ringPhase + index))
            }
        }

        Column {
            anchors.centerIn: parent
            spacing: 28
            width: parent.width - 64

            Item {
                id: badge
                width: 168
                height: 168
                anchors.horizontalCenter: parent.horizontalCenter

                // Outer dashed orbit
                Canvas {
                    id: orbit
                    anchors.fill: parent
                    onPaint: {
                        var ctx = getContext("2d")
                        ctx.reset()
                        var cx = width / 2, cy = height / 2
                        var r = width * 0.46
                        ctx.strokeStyle = "rgba(96, 165, 250, 0.35)"
                        ctx.lineWidth = 1.5
                        ctx.setLineDash([5, 7])
                        ctx.beginPath()
                        ctx.arc(cx, cy, r, splash.ringPhase, splash.ringPhase + Math.PI * 1.4)
                        ctx.stroke()

                        ctx.strokeStyle = "rgba(52, 211, 153, 0.45)"
                        ctx.setLineDash([])
                        ctx.beginPath()
                        ctx.arc(cx, cy, r * 0.78, -splash.ringPhase * 1.4, -splash.ringPhase * 1.4 + Math.PI * 0.9)
                        ctx.stroke()

                        ctx.strokeStyle = "rgba(255,255,255,0.12)"
                        ctx.beginPath()
                        ctx.arc(cx, cy, r * 0.58, 0, Math.PI * 2)
                        ctx.stroke()
                    }
                    Connections {
                        target: splash
                        function onRingPhaseChanged() { orbit.requestPaint() }
                    }
                }

                // Core emblem — app icon
                Rectangle {
                    anchors.centerIn: parent
                    width: parent.width * 0.55 + splash.pulse * parent.width * 0.03
                    height: width
                    radius: width / 2
                    color: "#0B1A2E"
                    border.color: Qt.rgba(1, 1, 1, 0.25)
                    border.width: 1
                    clip: true

                    Image {
                        anchors.fill: parent
                        anchors.margins: parent.width * 0.08
                        source: "qrc:/resources/icons/app_icon_256.png"
                        fillMode: Image.PreserveAspectFit
                        smooth: true
                        mipmap: true
                    }
                }

                // Orbiting dots
                Repeater {
                    model: 3
                    delegate: Rectangle {
                        required property int index
                        width: 8; height: 8; radius: 4
                        color: index === 1 ? "#34D399" : "#60A5FA"
                        property real ang: splash.ringPhase * (1.0 + index * 0.35) + index * 2.1
                        x: badge.width / 2 + Math.cos(ang) * (62 + index * 8) - width / 2
                        y: badge.height / 2 + Math.sin(ang) * (62 + index * 8) - height / 2
                    }
                }
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "FAMILY COIN"
                color: "#E2E8F0"
                font.pixelSize: 22
                font.bold: true
                font.letterSpacing: 4
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Семейный бюджет"
                color: "#94A3B8"
                font.pixelSize: 13
            }

            // Progress track
            Item {
                width: parent.width
                height: 46

                Rectangle {
                    id: track
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    height: 8
                    radius: 4
                    color: "#1E293B"

                    // Soft pulse halo when waiting on long loads
                    Rectangle {
                        anchors.centerIn: parent
                        width: parent.width + 10
                        height: parent.height + 10
                        radius: 6
                        z: -1
                        visible: splash.holdingForLoad
                        color: "transparent"
                        border.width: 2
                        border.color: Qt.rgba(0.23, 0.51, 0.96, 0.15 + splash.pulse * 0.35)
                    }

                    Rectangle {
                        id: fill
                        width: Math.max(12, track.width * (SplashCtrl ? SplashCtrl.progress : 0.05))
                        height: parent.height
                        radius: 4
                        gradient: Gradient {
                            orientation: Gradient.Horizontal
                            GradientStop { position: 0.0; color: "#2563EB" }
                            GradientStop { position: 0.55; color: "#3B82F6" }
                            GradientStop { position: 1.0; color: "#34D399" }
                        }

                        Behavior on width {
                            NumberAnimation { duration: 280; easing.type: Easing.OutCubic }
                        }

                        // Shimmer
                        Rectangle {
                            width: 28
                            height: parent.height
                            radius: 4
                            opacity: 0.35 + splash.pulse * 0.25
                            gradient: Gradient {
                                orientation: Gradient.Horizontal
                                GradientStop { position: 0.0; color: "transparent" }
                                GradientStop { position: 0.5; color: "#FFFFFF" }
                                GradientStop { position: 1.0; color: "transparent" }
                            }
                            x: (fill.width - width) * splash.pulse
                        }
                    }
                }

                Text {
                    anchors.right: parent.right
                    anchors.top: track.bottom
                    anchors.topMargin: 10
                    text: Math.round((SplashCtrl ? SplashCtrl.progress : 0) * 100) + "%"
                    color: "#64748B"
                    font.pixelSize: 11
                    font.bold: true
                }
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                text: SplashCtrl ? SplashCtrl.statusText : "Загрузка…"
                color: "#CBD5E1"
                font.pixelSize: 13
                elide: Text.ElideRight
            }
        }
    }

    Component.onCompleted: {
        SplashCtrl.markSplashVisible()
        SplashCtrl.setStatus("Подготовка окружения…")
    }
}
