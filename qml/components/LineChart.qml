import QtQuick
import FamilyBudget 1.0
import components 1.0

Item {
    id: root
    property var seriesA: [] // [{month, valueCents}]
    property var seriesB: []
    property color colorA: ThemeManager.accentColor
    property color colorB: ThemeManager.successColor
    property bool sharedScale: true

    onSeriesAChanged: canvas.requestPaint()
    onSeriesBChanged: canvas.requestPaint()
    onSharedScaleChanged: canvas.requestPaint()

    Canvas {
        id: canvas
        anchors.fill: parent
        anchors.margins: UiTheme.spaceSm

        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()
            var w = width
            var h = height
            if (w < 2 || h < 2)
                return

            ctx.strokeStyle = ThemeManager.borderColor.toString()
            ctx.lineWidth = 1
            ctx.beginPath()
            ctx.moveTo(0, h - 1)
            ctx.lineTo(w, h - 1)
            ctx.stroke()

            // light horizontal guides
            ctx.globalAlpha = 0.35
            for (var g = 1; g <= 3; ++g) {
                var gy = h * g / 4
                ctx.beginPath()
                ctx.moveTo(0, gy)
                ctx.lineTo(w, gy)
                ctx.stroke()
            }
            ctx.globalAlpha = 1.0

            function boundsOf(data) {
                if (!data || data.length === 0)
                    return null
                var minV = data[0].valueCents
                var maxV = data[0].valueCents
                for (var i = 1; i < data.length; ++i) {
                    minV = Math.min(minV, data[i].valueCents)
                    maxV = Math.max(maxV, data[i].valueCents)
                }
                return { minV: minV, maxV: maxV }
            }

            var bA = boundsOf(root.seriesA)
            var bB = boundsOf(root.seriesB)
            var minV = 0
            var maxV = 1
            if (root.sharedScale) {
                var has = false
                if (bA) { minV = bA.minV; maxV = bA.maxV; has = true }
                if (bB) {
                    if (!has) { minV = bB.minV; maxV = bB.maxV; has = true }
                    else { minV = Math.min(minV, bB.minV); maxV = Math.max(maxV, bB.maxV) }
                }
                if (!has)
                    return
            }

            function drawSeries(data, color, localScale) {
                if (!data || data.length < 2)
                    return
                var lo = minV
                var hi = maxV
                if (!root.sharedScale || localScale) {
                    var b = boundsOf(data)
                    lo = b.minV
                    hi = b.maxV
                }
                if (hi === lo)
                    hi = lo + 1
                ctx.strokeStyle = color
                ctx.lineWidth = Math.max(2, UiTheme.u * 0.22)
                ctx.lineJoin = "round"
                ctx.lineCap = "round"
                ctx.beginPath()
                for (var j = 0; j < data.length; ++j) {
                    var x = j / (data.length - 1) * w
                    var y = h - ((data[j].valueCents - lo) / (hi - lo)) * (h - 10) - 5
                    if (j === 0)
                        ctx.moveTo(x, y)
                    else
                        ctx.lineTo(x, y)
                }
                ctx.stroke()
            }

            drawSeries(root.seriesA, root.colorA.toString(), false)
            drawSeries(root.seriesB, root.colorB.toString(), false)
        }
    }

    Connections {
        target: ThemeManager
        function onThemeChanged() { canvas.requestPaint() }
    }
}
