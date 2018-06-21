import QtQuick 2.5
import QtQuick.Controls 1.5

Canvas {
    id: canvasPolyLines;
    anchors.fill: parent;
    opacity: 0.5;
    renderStrategy: Canvas.Threaded;
    focus: false;
    activeFocusOnTab: false;

    property int linesCount: 0;
    property var lineWidth;
    property var lineColor;
    property var pointsCountPerLine;
    property var pointsCoordXPerLine;
    property var pointsCoordYPerLine;

    onPaint: {
        if(parent.bIsStart) {
            var ctx = canvasPolyLines.getContext("2d");
            for(var li = 0; li < linesCount; li++) {
                ctx.lineWidth = lineWidth[li];
                ctx.strokeStyle = lineColor[li];
                ctx.beginPath();
                ctx.moveTo(width * pointsCoordXPerLine[li][0], height * pointsCoordYPerLine[li][0]);
                for(var lj = 1; lj < pointsCountPerLine[li]; lj++) {
                    ctx.lineTo(width * pointsCoordXPerLine[li][lj], height * pointsCoordYPerLine[li][lj]);
                }
                ctx.stroke();
            }
        }
    }
}
