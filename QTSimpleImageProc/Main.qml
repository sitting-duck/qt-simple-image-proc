import QtQuick
import QtQuick.Window

Window {
    width: 800
    height: 500
    visible: true
    title: "QTSimpleImageProc"

    Rectangle {
        anchors.fill: parent
        color: "#202020"
    }

    ShaderEffect {
        anchors.centerIn: parent
        width: 320
        height: 240

        property color tintColor: "#33ccff"

        fragmentShader: "qrc:/shaders/basic.frag.qsb"
    }
}