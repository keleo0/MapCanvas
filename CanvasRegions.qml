import QtQuick 2.5
import QtQuick.Controls 1.5

Canvas {
    id: canvasRegions;
    anchors.fill: parent;
    opacity: 0.5;
    renderStrategy: Canvas.Threaded;
    focus: false;
    activeFocusOnTab: false;

    property int regionsCount: 0;
    property var regionsBorderWidth;
    property var regionsBorderColor;
    property var regionsInnerColor;
    property var pointsCountPerRegion;
    property var pointsCoordXPerRegion;
    property var pointsCoordYPerRegion;

    onPaint: {
        if(parent.bIsStart) {
            var ctx = canvasRegions.getContext("2d");
            for(var ri = 0; ri < regionsCount; ri++) {
                ctx.lineWidth = regionsBorderWidth[ri];
                ctx.strokeStyle = regionsBorderColor[ri];
                ctx.fillStyle = regionsInnerColor[ri];
                ctx.beginPath();
                ctx.moveTo(width * pointsCoordXPerRegion[ri][0], height * pointsCoordYPerRegion[ri][0]);
                for(var rj = 1; rj < pointsCountPerRegion[ri]; rj++) {
                    ctx.lineTo(width * pointsCoordXPerRegion[ri][rj], height * pointsCoordYPerRegion[ri][rj]);
                }
                ctx.closePath();
                ctx.fill();
                ctx.stroke();
            }
        }
    }
}
