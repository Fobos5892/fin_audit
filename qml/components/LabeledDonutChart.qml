import QtQuick
import QtQuick.Layouts
import FamilyBudget 1.0
import components 1.0

Item {
    id: root
    property var slices: [] // [{label, color, value}]
    property string emptyText: "Нет данных"

    readonly property real total: {
        var s = 0
        var data = root.slices || []
        for (var i = 0; i < data.length; ++i)
            s += Number(data[i].value)
        return s
    }

    RowLayout {
        anchors.fill: parent
        spacing: UiTheme.spaceMd

        Item {
            Layout.fillHeight: true
            Layout.preferredWidth: Math.min(height, parent.width * 0.48)
            Layout.minimumWidth: UiTheme.u * 14

            Canvas {
                id: canvas
                anchors.fill: parent
                anchors.margins: UiTheme.spaceXs

                onPaint: {
                    var ctx = getContext("2d")
                    ctx.reset()
                    var cx = width / 2
                    var cy = height / 2
                    var radius = Math.min(width, height) / 2 - 2
                    var inner = radius * 0.58
                    var data = root.slices || []
                    var sum = root.total

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
                        var sweep = (Number(data[i].value) / sum) * Math.PI * 2
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
                target: root
                function onSlicesChanged() { canvas.requestPaint() }
                function onTotalChanged() { canvas.requestPaint() }
            }
            Connections {
                target: ThemeManager
                function onThemeChanged() { canvas.requestPaint() }
            }
        }

        Flickable {
            Layout.fillWidth: true
            Layout.fillHeight: true
            contentHeight: legendCol.implicitHeight
            clip: true
            boundsBehavior: Flickable.StopAtBounds
            flickableDirection: Flickable.VerticalFlick

            ColumnLayout {
                id: legendCol
                width: parent.width
                spacing: UiTheme.spaceXs

                Text {
                    visible: root.total <= 0
                    text: root.emptyText
                    color: ThemeManager.mutedTextColor
                    font.pixelSize: UiTheme.fontSm
                }

                Repeater {
                    model: root.slices
                    delegate: RowLayout {
                        Layout.fillWidth: true
                        spacing: UiTheme.spaceSm
                        required property var modelData

                        Rectangle {
                            Layout.preferredWidth: UiTheme.spaceSm
                            Layout.preferredHeight: UiTheme.spaceSm
                            radius: width / 2
                            color: modelData.color || ThemeManager.accentColor
                        }
                        Text {
                            text: modelData.label || ""
                            color: ThemeManager.textColor
                            font.pixelSize: UiTheme.fontSm
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }
                        Text {
                            text: root.total > 0
                                  ? (Math.round(Number(modelData.value) / root.total * 1000) / 10) + "%"
                                  : "0%"
                            color: ThemeManager.mutedTextColor
                            font.pixelSize: UiTheme.fontSm
                            font.bold: true
                        }
                    }
                }
            }
        }
    }
}
