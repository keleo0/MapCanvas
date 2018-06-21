import QtQuick 2.5
import QtQuick.Controls 1.5

Canvas {
    id: canvasTexts;
    anchors.fill: parent;
    opacity: 0.5;
    renderStrategy: Canvas.Threaded;
    focus: false;
    activeFocusOnTab: false;

    property int textsCount: 0;
    property var textsList;
    property var textsCoordList;

    onPaint: {
        if(parent.bIsStart) {
            var ctx = canvasTexts.getContext("2d");
            ctx.lineWidth = 1;
            ctx.strokeStyle = "black";
            ctx.fillStyle = "black";
            ctx.font = "20px sans-serif";
            for(var ti = 0; ti < textsCount; ti++) {
                ctx.beginPath();
                ctx.text(textsList[ti], width * textsCoordList[ti].x, height * textsCoordList[ti].y);
                ctx.stroke();
                ctx.fill();
            }
        }
    }
}
