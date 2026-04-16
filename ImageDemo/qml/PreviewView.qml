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
            id: previewSurface
            objectName: "previewSurface"
            anchors.fill: parent
            anchors.margins: 16

            Image {
                id: sourceImage
                objectName: "sourceImage"
                anchors.fill: parent

                source: {
                    const path = effectSettings.imagePath ? effectSettings.imagePath.trim() : ""

                    if (path === "") {
                        return ""
                    }

                    if (path.startsWith("http://") ||
                        path.startsWith("https://") ||
                        path.startsWith("file:///") ||
                        path.startsWith("qrc:/")) {
                        return path
                    }

                    return "file:///" + path
                }

                fillMode: Image.PreserveAspectFit
                smooth: true
                visible: false
            }

            ShaderEffect {
                id: previewEffect
                objectName: "previewEffect"
                anchors.fill: parent
                visible: effectSettings.imagePath !== "" && sourceImage.status === Image.Ready

                property variant source: sourceImage
                property real blurRadius: effectSettings.blurRadius
                property real effectOpacity: effectSettings.opacity
                property real effectEnabled: effectSettings.enabled ? 1.0 : 0.0

                property real texelStepX: sourceImage.status === Image.Ready && sourceImage.implicitWidth > 0
                                          ? 1.0 / sourceImage.implicitWidth : 0.0
                property real texelStepY: sourceImage.status === Image.Ready && sourceImage.implicitHeight > 0
                                          ? 1.0 / sourceImage.implicitHeight : 0.0

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