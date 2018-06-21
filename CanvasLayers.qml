import QtQuick 2.5
import QtQuick.Controls 1.5

Canvas {
    id: canvasLayer;
    anchors.fill: parent;
    opacity: 0.5;
    renderStrategy: Canvas.Threaded;
    focus: false;
    activeFocusOnTab: false;

    property real gAlpha: 0.5;

    //property bool drawSwitch: false;
    property int drawType: 0; //1:points, 2:lines, 3:region, 4:text

    property int penWidth: 0;
    property color penColor: "transparent";
    property color paletteColor: "transparent";
    property int pointSide: 3;
    property int pointsCount: 0;
    property var pointsList;

    property int linesCount: 0;
    property var lineWidth;
    property var lineColor;
    property var pointsCountPerLine;
    property var pointsCoordXPerLine;
    property var pointsCoordYPerLine;

    property int regionsCount: 0;
    property var regionNames;
    property var regionCenterCoords;
    property var regionsBorderWidth;
    property var regionsBorderColor;
    property var regionsInnerColor;
    property var pointsCountPerRegion;
    property var pointsCoordXPerRegion;
    property var pointsCoordYPerRegion;

    property int textsCount: 0;
    property var textsList;
    property var textsCoordList;

    onPaint: {
        //console.log("paint:", drawType)
        var ctx = canvasLayer.getContext("2d");
        switch(drawType) {
        case 1: //points
            ctx.globalAlpha = gAlpha;
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
            break;
        case 2: //lines
            ctx.globalAlpha = gAlpha;
            for(var li = 0; li < linesCount; li++) {
                //console.log(lineWidth[li], lineColor[li], linesCount);
                ctx.lineWidth = lineWidth[li];
                ctx.strokeStyle = lineColor[li];
                ctx.beginPath();
                ctx.moveTo(width * pointsCoordXPerLine[li][0], height * pointsCoordYPerLine[li][0]);
                for(var lj = 1; lj < pointsCountPerLine[li]; lj++) {
                    ctx.lineTo(width * pointsCoordXPerLine[li][lj], height * pointsCoordYPerLine[li][lj]);
                }
                ctx.stroke();
            }
            break;
        case 3: //regions
            for(var ri = 0; ri < regionsCount; ri++) {
                //draw regions
                ctx.globalAlpha = gAlpha;
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

                //draw regions name
                ctx.globalAlpha = 1;
                //ctx.lineWidth = 1;
                //ctx.strokeStyle = "black";
                ctx.fillStyle = "black";
                ctx.font = "18px sans-serif";
                ctx.beginPath();
                ctx.text(regionNames[ri], width * regionCenterCoords[ri].x, height * regionCenterCoords[ri].y);
                //ctx.stroke();
                ctx.fill();
            }
            break;
        case 4: //texts
            ctx.lineWidth = 1;
            ctx.strokeStyle = "black";
            ctx.fillStyle = "red";
            //ctx.font = "18px";
            for(var ti = 0; ti < textsCount; ti++) {
                ctx.beginPath();
                ctx.text(textsList[ti], width * textsCoordList[ti].x, height * textsCoordList[ti].y);
                ctx.stroke();
                ctx.fill();
            }
            break;
        default:
            console.log("Unknown canvas draw type:", drawType)
            break;
        }
    }
}
