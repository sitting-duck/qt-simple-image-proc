import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Window {
    width: 1100
    height: 900
    visible: true
    title: "QTSimpleImageProc"

    Rectangle {
        anchors.fill: parent
        color: "#202020"
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 220
            radius: 8
            color: "#2a2a2a"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 16

                    Text {
                        text: "Grayscale"
                        color: "white"
                        font.pixelSize: 16
                        Layout.preferredWidth: 100
                    }

                    Slider {
                        id: grayscaleSlider
                        Layout.fillWidth: true
                        from: 0.0
                        to: 1.0
                        value: 0.0
                    }

                    Text {
                        text: grayscaleSlider.value.toFixed(2)
                        color: "white"
                        font.pixelSize: 14
                        Layout.preferredWidth: 50
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 16

                    Text {
                        text: "Invert"
                        color: "white"
                        font.pixelSize: 16
                        Layout.preferredWidth: 100
                    }

                    Slider {
                        id: invertSlider
                        Layout.fillWidth: true
                        from: 0.0
                        to: 1.0
                        value: 0.0
                    }

                    Text {
                        text: invertSlider.value.toFixed(2)
                        color: "white"
                        font.pixelSize: 14
                        Layout.preferredWidth: 50
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 16

                    Text {
                        text: "Brightness"
                        color: "white"
                        font.pixelSize: 16
                        Layout.preferredWidth: 100
                    }

                    Slider {
                        id: brightnessSlider
                        Layout.fillWidth: true
                        from: -1.0
                        to: 1.0
                        value: 0.0
                    }

                    Text {
                        text: brightnessSlider.value.toFixed(2)
                        color: "white"
                        font.pixelSize: 14
                        Layout.preferredWidth: 50
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 16

                    Text {
                        text: "Contrast"
                        color: "white"
                        font.pixelSize: 16
                        Layout.preferredWidth: 100
                    }

                    Slider {
                        id: contrastSlider
                        Layout.fillWidth: true
                        from: 0.0
                        to: 2.0
                        value: 1.0
                    }

                    Text {
                        text: contrastSlider.value.toFixed(2)
                        color: "white"
                        font.pixelSize: 14
                        Layout.preferredWidth: 50
                    }
                }
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Image {
                id: sourceImage
                source: "qrc:/qt/qml/QTSimpleImageProc/images/test.png"
                visible: false
                smooth: true
            }

            ShaderEffectSource {
                id: sourceTexture
                sourceItem: sourceImage
                hideSource: true
                live: true
                smooth: true
            }

            ShaderEffect {
                anchors.centerIn: parent
                width: 900
                height: 600

                property var source: sourceTexture
                property vector4d controls1: Qt.vector4d(
                    grayscaleSlider.value,
                    invertSlider.value,
                    brightnessSlider.value,
                    contrastSlider.value
                )

                fragmentShader: "qrc:/shaders/basic.frag.qsb"
            }
        }
    }
}