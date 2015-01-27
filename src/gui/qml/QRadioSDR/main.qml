import QtQuick 2.2
import QtQuick.Controls 1.1
import CustomPlot 1.0

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    title: "QRadioSDR"

    menuBar: MenuBar {
        Menu {
            title: "File"
            MenuItem { text: "Open..." }
            MenuItem { text: "Close" }
        }

        Menu {
            title: "Edit"
            MenuItem { text: "Cut" }
            MenuItem { text: "Copy" }
            MenuItem { text: "Paste" }
        }
    }

    toolBar: ToolBar {
        Row {
                    anchors.fill: parent
                    ToolButton {
                        iconSource: "icons/btn.png"
                    }
        }
    }

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
            value: 10
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
        Row {
            x: 19
            y: 9
            width: 490
            height: 11
            spacing: 5

            Switch { checked : false }

            ListView {
                width: 80; height: 10
                spacing: 1
                cacheBuffer: 20
                snapMode: ListView.SnapOneItem
                keyNavigationWraps: true
                orientation: ListView.Horizontal
                contentHeight: 10
                contentWidth: 80
                flickableDirection: Flickable.HorizontalFlick

                Component {
                    id: frequenciesDelegate
                    Rectangle {
                        id: wrapper
                        width: 80
                        height: freqInfo.height
                        //color: ListView.isCurrentItem ? "black" : "red"
                        Text {
                            id: freqInfo
                            text: freq + ": " + basef
                            //color: wrapper.ListView.isCurrentItem ? "red" : "black"
                        }
                    }
                }

                model: ListModel {
                    ListElement {
                        freq: "1000"
                        basef: "khz"
                    }
                    ListElement {
                        freq: "500"
                        basef: "khz"
                    }
                    ListElement {
                        freq: "100"
                        basef: "khz"
                    }
                    ListElement {
                        freq: "50"
                        basef: "khz"
                    }
                    ListElement {
                        freq: "25"
                        basef: "khz"
                    }
                }
                delegate: frequenciesDelegate
                focus: true
            }

        }
        CustomPlotItem {
            id: customPlot
            //anchors.fill: parent
            x: 5
            y: 26
            width: 514
            height: 181
            // Color settings
            background : "#000000"
            xAxis: CustomAxis {
                label : "Frequency [Mhz]"
                color : "#ffffff"
                min : 106.9
                max : 107.9
            }

            yAxis: CustomAxis {
                label : "Power [db]"
                color : "#ffffff"
                min : 0.0
                max : 50.0
            }

            Component.onCompleted: initCustomPlot()

        }
    }
    statusBar: StatusBar {
        id:status
        Row {
                anchors.fill: parent
                Label { text: "Read Only" }
            }
    }
}
