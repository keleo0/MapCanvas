import QtQuick 2.5
import QtQuick.Controls 1.5
import MapCanvasManager 1.0

ScrollView {
    id: ha661MapCanvas;
    activeFocusOnTab: true;

    MapManager {
        id: canvasScene;
        objectName: "mapCanvasScene";
        anchors.centerIn: parent;
        width: getDefaultSceneSize().width;
        height: getDefaultSceneSize().height;

        property bool bIsStart: false;  //是否可以开始绘制

        function createCanvasLayer(layerName){
            var component = Qt.createComponent("qrc:/" + layerName + ".qml");
            if (component.status == Component.Ready) {
                var layerObject = component.createObject(canvasScene,{"objectName": layerName});
                if(layerObject == null) {
                    console.log(component.errorString());
                    return;
                }
            }
            else
                console.log(component.errorString());
        }

        Component.onCompleted: {
            createLayers();
            doMapCanvasWork();
        }
    }

    Keys.enabled: true;
    Keys.onUpPressed: {
        canvasScene.scale += 0.2;
        console.log(canvasScene.width, canvasScene.height)
    }
    Keys.onDownPressed: {
        canvasScene.scale -= 0.2;
    }
//    Keys.onReturnPressed: {
//        canvasScene.doMapCanvasWork();
//    }
}

