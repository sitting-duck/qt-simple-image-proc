import QtQuick
import QtQuick.Controls

Rectangle {
    anchors.fill: parent
    color: "#202020"

    Rectangle {
        id: frame
        width: Math.min(parent.width * 0.85, 900)
        height: Math.min(parent.height * 0.85, 650)
        anchors.centerIn: parent
        radius: 12
        color: "#2a2a2a"
        border.color: "#808080"
        border.width: 1
        clip: true

        Item {
            anchors.fill: parent
            anchors.margins: 16

            Image {
                id: sourceImage
                anchors.fill: parent
                source: effectSettings.imagePath.startsWith("http://") ||
                        effectSettings.imagePath.startsWith("https://")
                        ? effectSettings.imagePath
                        : "file://" + effectSettings.imagePath
                fillMode: Image.PreserveAspectFit
                smooth: true
                visible: false
            }

            ShaderEffect {
                id: previewEffect
                anchors.fill: parent
                visible: effectSettings.imagePath !== ""

                property variant source: sourceImage
                property real blurRadius: effectSettings.blurRadius
                property real effectOpacity: effectSettings.opacity
                property real effectEnabled: effectSettings.enabled ? 1.0 : 0.0
                property real texelStepX: sourceImage.status === Image.Ready && sourceImage.paintedWidth > 0
                                          ? 1.0 / sourceImage.paintedWidth : 0.0
                property real texelStepY: sourceImage.status === Image.Ready && sourceImage.paintedHeight > 0
                                          ? 1.0 / sourceImage.paintedHeight : 0.0

                vertexShader: "qrc:/shaders/passthrough.vert.qsb"
                fragmentShader: "qrc:/shaders/blur.frag.qsb"
            }

            Column {
                anchors.centerIn: parent
                spacing: 10
                visible: effectSettings.imagePath === ""

                Text {
                    text: "No Image Loaded"
                    color: "white"
                    font.pixelSize: 28
                    horizontalAlignment: Text.AlignHCenter
                }

                Text {
                    text: "Use File > Open to load an image"
                    color: "#d0d0d0"
                    font.pixelSize: 18
                    horizontalAlignment: Text.AlignHCenter
                }

                Text {
                    text: "blurRadius: " + effectSettings.blurRadius.toFixed(1)
                    color: "#d0d0d0"
                    font.pixelSize: 16
                }

                Text {
                    text: "opacity: " + effectSettings.opacity.toFixed(2)
                    color: "#d0d0d0"
                    font.pixelSize: 16
                }

                Text {
                    text: "enabled: " + effectSettings.enabled
                    color: "#d0d0d0"
                    font.pixelSize: 16
                }
            }
        }
    }
}