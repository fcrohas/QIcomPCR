import QtQuick 2.0

Item {
    id: bargraph
    property real value: 20
    Row {
        anchors.fill: parent
        x: 0
        y: 0
        spacing:1
        Repeater {
            model: 100
            delegate: Rectangle {
                color : (index < 35) ? "#15f327" : (index < 75) ? "#ff9900" : "#ff0000"
                x: index * (100 * 4) / parent.width
                y: 0
                width: 1
                height : parent.height
            }
        }
    }
    // Hide 80 % opacity 100% complement signal level
    Rectangle {
        id: dark
        x: bargraph.value * parent.width / 100
        y: 0
        width : parent.width - bargraph.value * parent.width / 100
        height: parent.height
        opacity: 0.8
        color: "#000000"
    }

}

