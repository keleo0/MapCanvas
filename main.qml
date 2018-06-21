import QtQuick 2.6
import QtQuick.Controls 1.5

Item {
    id: rootItem;
    width: 1024;
    height: 768;

    MapCanvas {
        anchors.fill: parent;
    }
}
