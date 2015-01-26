import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.1

Window {
    visible: true
    width: 800
    height: 600
    title: "QRadioSDR"

    Grid {
        id: maingrid
        anchors.fill: parent
        transformOrigin: Item.Center
    }

    GroupBox {
        id: filterBox
        x: 674
        y: 44
        width: 114
        height: 161
        flat: false
        checkable: false
        title: qsTr("Filter")
        TextField {
            id: customFrequency
            x: 0
            y: 114
            width: 98
            height: 20
            placeholderText: qsTr("Enter frequency")
            enabled: false
        }

        Column {
            id: filterGroup
            x: 0
            y: 6
            ExclusiveGroup { id : filters }
            RadioButton {
                        id: radioFilter28
                        width: 98
                        height: 17
                        text: qsTr("2.8Khz")
                        exclusiveGroup: filters
                    }

            RadioButton {
                        id: radioFilter6
                        width: 98
                        height: 17
                        text: qsTr("6Khz")
                        exclusiveGroup: filters
                    }

            RadioButton {
                        id: radioFilter15
                        width: 98
                        height: 17
                        text: qsTr("15Khz")
                        checked: true
                        exclusiveGroup: filters
                    }

            RadioButton {
                        id: radioFilter50
                        width: 98
                        height: 17
                        text: qsTr("50Khz")
                        exclusiveGroup: filters
                    }

            RadioButton {
                        id: radioFilter230
                        width: 98
                        height: 17
                        text: qsTr("230Khz")
                        exclusiveGroup: filters
                    }

            RadioButton {
                id: radioFilterCustom
                width: 98
                height: 17
                text: qsTr("Custom")
                exclusiveGroup: filters
                onCheckedChanged: customFrequency.enabled = radioFilterCustom.checked
            }
        }
    }

    GroupBox {
        id: modulationBox
        x: 674
        y: 216
        width: 114
        height: 150
        title: qsTr("Modulation")

        Column {
            id: modulationGroup
            x: 0
            y: 4
            width: 98
            height: 120
            ExclusiveGroup { id : modulations }
            RadioButton {
                        id: radioModAM
                        width: 98
                        height: 17
                        text: qsTr("AM")
                        exclusiveGroup: modulations
                    }

            RadioButton {
                        id: radioModCW
                        width: 98
                        height: 17
                        text: qsTr("CW")
                        exclusiveGroup: modulations
                    }

            RadioButton {
                        id: radioMOD
                        width: 98
                        height: 17
                        text: qsTr("LSB")
                        exclusiveGroup: modulations
                    }

            RadioButton {
                        id: radioModUSB
                        width: 98
                        height: 17
                        text: qsTr("USB")
                        exclusiveGroup: modulations
                    }

            RadioButton {
                        id: radioModFM
                        width: 98
                        height: 17
                        text: qsTr("FM")
                        exclusiveGroup: modulations
                    }

            RadioButton {
                        id: radioModWBFM
                        width: 98
                        height: 17
                        text: qsTr("WBFM")
                        checked: true
                        exclusiveGroup: modulations
                    }

            RadioButton {
                id: radioModRAW
                width: 98
                height: 17
                text: qsTr("RAW")
                exclusiveGroup: modulations
            }
        }
    }

    Rectangle {
        id: display
        x: 141
        y: 44
        width: 527
        height: 203
        color: "#000000"
        radius: 19
        border.color: "#493b3b"
        border.width: 3

        Text {
            id: frequency1
            x: 19
            y: 22
            width: 209
            height: 28
            color: "#120ba7"
            text: qsTr("106 500 000")
            styleColor: "#ffffff"
            textFormat: Text.AutoText
            style: Text.Outline
            font.italic: false
            font.family: "Verdana"
            font.bold: false
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 29
        }
        Text {
            id: frequency2
            x: 295
            y: 22
            width: 209
            height: 28
            color: "#120ba7"
            text: qsTr("106 500 000")
            font.family: "Verdana"
            font.italic: false
            font.bold: false
            style: Text.Outline
            styleColor: "#ffffff"
            textFormat: Text.AutoText
            font.pixelSize: 29
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        Bargraph {
            id: signal1
            x: 19
            y: 56
            width: 209
            height: 10
            value: 20
        }
        Bargraph {
            id: signal2
            x: 295
            y: 56
            width: 209
            height: 10
            value: 75
        }
    }
    Rectangle {
        x: 26
        y: 44
        width: 109
        height: 466
        color: "#000000"
        radius: 19
        border.color: "#493b3b"
        border.width: 3

        Column {
            id: columnbar
            anchors.fill: parent
            spacing: 20
            Knob {
                id: squelchKnob
                label: "Squelch"
                zoomFactor: 0.5
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Knob {
                id: ifshiftKnob
                percentage: 0
                label: "IF Shift"
                zoomFactor: 0.5
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Knob {
                id: gainKnob
                label: "Gain"
                zoomFactor: 0.5
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Knob {
                id: volumeKnob
                label: "Volume"
                zoomFactor: 1.0
                anchors.horizontalCenter: parent.horizontalCenter
            }

        }
    }
    Rectangle {
        id: rectangle1
        x: 141
        y: 295
        width: 527
        height: 215
        color: "#000000"
        radius: 19
        border.color: "#493b3b"
        border.width: 3
    }
}
