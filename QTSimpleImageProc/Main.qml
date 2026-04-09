import QtQuick
import QtQuick.Window

Window {
    width: 1000
    height: 700
    visible: true
    title: "QTSimpleImageProc"

    Rectangle {
        anchors.fill: parent
        color: "#202020"
    }

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
        width: 640
        height: 480

        property var source: sourceTexture

        fragmentShader: "qrc:/shaders/basic.frag.qsb"
    }
}