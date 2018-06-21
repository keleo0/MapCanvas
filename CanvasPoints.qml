import QtQuick 2.5
import QtQuick.Controls 1.5

Canvas {
    id: canvasPoints;
    anchors.fill: parent;
    opacity: 0.5;
    renderStrategy: Canvas.Threaded;
    focus: false;
    activeFocusOnTab: false;

    property int penWidth: 0;
    property color penColor: "transparent";
    property color paletteColor: "transparent";
    property int pointSide: 3;
    property int pointsCount: 0;
    property var pointsList;

    onPaint: {
        if(parent.bIsStart) {
            var ctx = canvasPoints.getContext("2d");
            ctx.lineWidth = penWidth;
            ctx.strokeStyle = penColor;
            ctx.fillStyle = "yellow";
            for(var pi = 0; pi < pointsCount; pi++) {
                var x = width * pointsList[pi].x;
                var y = height * pointsList[pi].y;
                ctx.beginPath();
                ctx.moveTo(x-pointSide, y);
                ctx.lineTo(x, y+pointSide);
                ctx.lineTo(x+pointSide, y);
                ctx.lineTo(x, y-pointSide);
                ctx.lineTo(x-pointSide, y);
                ctx.closePath();
                ctx.fill();
                ctx.stroke();
            }
        }
    }
}
