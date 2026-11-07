import QtQuick
import FamilyBudget 1.0
import components 1.0

Item {
    id: root
    // Доходы: работа + пассив (пассив может быть < 0)
    property real workCents: 0
    property real passiveCents: 0
    property real expenseCents: 0

    readonly property real positivePassive: Math.max(0, passiveCents)
    readonly property real negativePassive: Math.min(0, passiveCents)
    readonly property real budgetCents: Math.max(1, workCents + positivePassive)
    readonly property real expensePct: (expenseCents / budgetCents) * 100
    readonly property real passiveNegPct: (Math.abs(negativePassive) / budgetCents) * 100

    property color workColor: "#7C6BC4"
    property color passiveColor: "#5B8FD9"
    property color passiveNegColorA: "#7DD3FC"
    property color passiveNegColorB: "#2563EB"
    property color underColor: "#3FAE8A"
    property color overColorStart: "#E07A7A"
    property color overColorEnd: "#7A1F2B"

    Canvas {
        id: canvas
        anchors.fill: parent
        anchors.margins: UiTheme.spaceSm

        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()
            var cx = width / 2
            var cy = height / 2
            var maxR = Math.min(width, height) / 2 - 2
            var trackW = Math.max(8, maxR * 0.12)
            var incomeR = maxR * 0.62
            var expenseR = maxR * 0.88

            function ring(r, width, start, sweep, color) {
                if (sweep <= 0.0001)
                    return
                ctx.beginPath()
                ctx.lineWidth = width
                ctx.lineCap = "round"
                ctx.strokeStyle = color
                ctx.arc(cx, cy, r, start, start + sweep, false)
                ctx.stroke()
            }

            function mix(c1, c2, t) {
                function parse(c) {
                    c = ("" + c).replace("#", "")
                    if (c.length === 8) c = c.substring(0, 6)
                    return {
                        r: parseInt(c.substring(0, 2), 16),
                        g: parseInt(c.substring(2, 4), 16),
                        b: parseInt(c.substring(4, 6), 16)
                    }
                }
                var a = parse(c1), b = parse(c2)
                t = Math.max(0, Math.min(1, t))
                function h(n) {
                    var s = Math.round(n).toString(16)
                    return s.length < 2 ? "0" + s : s
                }
                return "#" + h(a.r + (b.r - a.r) * t) + h(a.g + (b.g - a.g) * t) + h(a.b + (b.b - a.b) * t)
            }

            // Background tracks
            ring(incomeR, trackW * 0.85, 0, Math.PI * 2, ThemeManager.borderColor.toString())
            ring(expenseR, trackW, 0, Math.PI * 2, ThemeManager.borderColor.toString())

            var budget = Math.max(1, root.budgetCents)
            var workSweep = (root.workCents / budget) * Math.PI * 2
            var passPosSweep = (root.positivePassive / budget) * Math.PI * 2
            var passNegSweep = (Math.abs(root.negativePassive) / budget) * Math.PI * 2
            var start = -Math.PI / 2

            // Базовый доход (работа) — фиолетовый
            ring(incomeR, trackW * 0.85, start, workSweep, root.workColor)
            // Пассивный положительный — синий
            ring(incomeR, trackW * 0.85, start + workSweep, passPosSweep, root.passiveColor)
            // Пассивный отрицательный — в обратную сторону, градиент голубой→синий
            if (passNegSweep > 0) {
                var steps = Math.max(8, Math.floor(passNegSweep * 18))
                var step = passNegSweep / steps
                for (var i = 0; i < steps; ++i) {
                    var t = i / Math.max(1, steps - 1)
                    // рисуем против часовой от start
                    ctx.beginPath()
                    ctx.lineWidth = trackW * 0.85
                    ctx.lineCap = "butt"
                    ctx.strokeStyle = mix(root.passiveNegColorA, root.passiveNegColorB, t)
                    ctx.arc(cx, cy, incomeR, start - i * step, start - (i + 1) * step, true)
                    ctx.stroke()
                }
            }

            // Расходы относительно бюджета (100% = полный круг expenseR)
            var expRatio = root.expenseCents / budget
            if (expRatio <= 1.0) {
                ring(expenseR, trackW, start, expRatio * Math.PI * 2, root.underColor)
            } else {
                // Первый полный круг — красный
                ring(expenseR, trackW, start, Math.PI * 2, root.overColorStart)
                // Дополнительные кольца за каждые +100%
                var over = expRatio - 1.0
                var ringIndex = 0
                while (over > 0.001 && ringIndex < 4) {
                    var part = Math.min(1.0, over)
                    var tone = Math.min(1.0, (ringIndex + part) / 4.0)
                    var col = mix(root.overColorStart, root.overColorEnd, tone)
                    var r = expenseR + (ringIndex + 1) * (trackW * 0.72)
                    if (r + trackW * 0.5 > maxR)
                        break
                    ring(r, trackW * 0.7, start, part * Math.PI * 2, col)
                    over -= 1.0
                    ringIndex++
                }
            }
        }
    }

    Column {
        anchors.centerIn: parent
        spacing: UiTheme.spaceXs * 0.5
        width: parent.width * 0.42

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: Math.round(root.expensePct) + "%"
            color: root.expensePct <= 100 ? root.underColor : root.overColorStart
            font.pixelSize: UiTheme.fontXl
            font.bold: true
        }
        Text {
            visible: root.negativePassive < 0
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            text: "пассив " + (root.passiveNegPct > 0 ? "−" : "+")
                  + Math.round(root.passiveNegPct) + "%"
            color: root.passiveNegColorB
            font.pixelSize: Math.max(10, UiTheme.fontSm * 0.85)
            font.bold: true
        }
    }

    Connections {
        target: root
        function onWorkCentsChanged() { canvas.requestPaint() }
        function onPassiveCentsChanged() { canvas.requestPaint() }
        function onExpenseCentsChanged() { canvas.requestPaint() }
    }
    Connections {
        target: ThemeManager
        function onThemeChanged() { canvas.requestPaint() }
    }
    onWidthChanged: canvas.requestPaint()
    onHeightChanged: canvas.requestPaint()
}
