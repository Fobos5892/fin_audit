import QtQuick
import FamilyBudget 1.0

Item {
    id: root
    property var slices: []

    onSlicesChanged: {
        anim.from = 0
        anim.to = 1
        anim.restart()
        canvas.requestPaint()
    }

    property real animProgress: 0
    NumberAnimation on animProgress {
        id: anim
        from: 0; to: 1
        duration: 650
        easing.type: Easing.OutCubic
        running: true
    }
    onAnimProgressChanged: canvas.requestPaint()

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()
            var cx = width / 2
            var cy = height / 2
            var radius = Math.min(width, height) / 2 - 4
            var inner = radius * 0.62
            var data = root.slices || []
            var sum = 0
            for (var s = 0; s < data.length; ++s)
                sum += Number(data[s].value)

            if (sum <= 0) {
                ctx.beginPath()
                ctx.arc(cx, cy, radius, 0, Math.PI * 2)
                ctx.fillStyle = ThemeManager.borderColor.toString()
                ctx.fill()
                ctx.beginPath()
                ctx.arc(cx, cy, inner, 0, Math.PI * 2)
                ctx.fillStyle = ThemeManager.surfaceColor.toString()
                ctx.fill()
                return
            }

            var angle = -Math.PI / 2
            for (var i = 0; i < data.length; ++i) {
                var sweep = (Number(data[i].value) / sum) * Math.PI * 2 * root.animProgress
                ctx.beginPath()
                ctx.moveTo(cx, cy)
                ctx.arc(cx, cy, radius, angle, angle + sweep)
                ctx.closePath()
                ctx.fillStyle = data[i].color
                ctx.fill()
                angle += sweep
            }
            ctx.beginPath()
            ctx.arc(cx, cy, inner, 0, Math.PI * 2)
            ctx.fillStyle = ThemeManager.surfaceColor.toString()
            ctx.fill()
        }
    }

    Connections {
        target: ThemeManager
        function onThemeChanged() { canvas.requestPaint() }
    }
}
