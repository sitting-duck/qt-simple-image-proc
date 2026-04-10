import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Window {
    width: 1420
    height: 960
    visible: true
    title: "QTSimpleImageProc"

    Rectangle {
        anchors.fill: parent
        color: "#202020"
    }

    component SectionPanel : Rectangle {
        id: sectionRoot

        property alias title: titleText.text
        property bool expanded: true
        default property alias sectionContent: contentColumn.data

        width: 300
        radius: 8
        color: "#2a2a2a"
        border.color: "#3a3a3a"
        border.width: 1
        implicitHeight: headerRow.height + (expanded ? contentColumn.implicitHeight + 16 : 0) + 16

        Column {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 10

            Rectangle {
                id: headerRow
                width: parent.width
                height: 34
                radius: 6
                color: "#333333"

                Row {
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    anchors.rightMargin: 10
                    spacing: 8

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: sectionRoot.expanded ? "▼" : "▶"
                        color: "white"
                        font.pixelSize: 14
                    }

                    Text {
                        id: titleText
                        anchors.verticalCenter: parent.verticalCenter
                        color: "white"
                        font.pixelSize: 15
                        font.bold: true
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: sectionRoot.expanded = !sectionRoot.expanded
                }
            }

            Column {
                id: contentColumn
                width: parent.width
                spacing: 12
                visible: sectionRoot.expanded
            }
        }
    }

    component SliderRow : Column {
        id: sliderRowRoot

        property alias label: labelText.text
        property alias helper: helperText.text
        property alias slider: controlSlider

        width: 280
        spacing: 6

        Text {
            id: labelText
            color: "white"
            font.pixelSize: 15
        }

        Slider {
            id: controlSlider
            width: parent.width
        }

        Row {
            width: parent.width
            spacing: 8

            Text {
                id: helperText
                width: parent.width - 60
                wrapMode: Text.WordWrap
                color: "#bdbdbd"
                font.pixelSize: 12
                visible: text.length > 0
            }

            Text {
                text: controlSlider.value.toFixed(2)
                color: "#d7d7d7"
                font.pixelSize: 12
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        Rectangle {
            Layout.preferredWidth: 220
            Layout.fillHeight: true
            radius: 8
            color: "#252525"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 14

                Text {
                    text: "Browse"
                    color: "white"
                    font.pixelSize: 22
                    font.bold: true
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    radius: 6
                    color: "#353535"

                    Text {
                        anchors.centerIn: parent
                        text: "Cataloged Folder"
                        color: "white"
                        font.pixelSize: 14
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    radius: 6
                    color: "#353535"

                    Text {
                        anchors.centerIn: parent
                        text: "This Computer"
                        color: "white"
                        font.pixelSize: 14
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 56
                    radius: 6
                    color: "#303030"

                    Text {
                        anchors.centerIn: parent
                        text: "test.png"
                        color: "#e0e0e0"
                        font.pixelSize: 14
                    }
                }

                Item { Layout.fillHeight: true }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 8
            color: "#1c1c1c"

            Column {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                Row {
                    spacing: 10

                    Rectangle {
                        width: 90
                        height: 32
                        radius: 6
                        color: "#3a3a3a"

                        Text {
                            anchors.centerIn: parent
                            text: "Browse"
                            color: "white"
                            font.pixelSize: 13
                        }
                    }

                    Rectangle {
                        width: 90
                        height: 32
                        radius: 6
                        color: "#4a4a4a"

                        Text {
                            anchors.centerIn: parent
                            text: "Develop"
                            color: "white"
                            font.pixelSize: 13
                            font.bold: true
                        }
                    }

                    Rectangle {
                        width: 90
                        height: 32
                        radius: 6
                        color: "#3a3a3a"

                        Text {
                            anchors.centerIn: parent
                            text: "Effects"
                            color: "white"
                            font.pixelSize: 13
                        }
                    }
                }

                Item {
                    width: parent.width
                    height: parent.height - 44

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
                        id: editorEffect
                        anchors.centerIn: parent
                        width: Math.min(parent.width - 40, (parent.height - 40) * 1.5)
                        height: width * 2 / 3

                        property var source: sourceTexture

                        property vector4d controls1: Qt.vector4d(
                            grayscaleSlider.slider.value,
                            invertSlider.slider.value,
                            brightnessSlider.slider.value,
                            contrastSlider.slider.value
                        )

                        property vector4d controls2: Qt.vector4d(
                            thresholdAmountSlider.slider.value,
                            thresholdCutoffSlider.slider.value,
                            saturationSlider.slider.value,
                            blurAmountSlider.slider.value
                        )

                        property vector4d controls3: Qt.vector4d(
                            1.0 / width,
                            1.0 / height,
                            effectOpacitySlider.slider.value,
                            blurRadiusSlider.slider.value
                        )

                        fragmentShader: "qrc:/shaders/basic.frag.qsb"
                    }
                }
            }
        }

        Rectangle {
            Layout.preferredWidth: 330
            Layout.fillHeight: true
            radius: 8
            color: "#252525"

            ScrollView {
                anchors.fill: parent
                anchors.margins: 12
                clip: true

                Column {
                    width: 290
                    spacing: 14

                    Text {
                        text: "Develop"
                        color: "white"
                        font.pixelSize: 22
                        font.bold: true
                    }

                    SectionPanel {
                        width: parent.width
                        title: "Effect Stack"

                        Text {
                            width: parent.width
                            wrapMode: Text.WordWrap
                            color: "#cfcfcf"
                            font.pixelSize: 12
                            text: "This is a non-destructive stack. Effects run top to bottom, so order matters."
                        }

                        Text {
                            width: parent.width
                            wrapMode: Text.WordWrap
                            color: "#bdbdbd"
                            font.pixelSize: 12
                            text: "Current order: Blur → Black & White → Invert → Brightness → Contrast → Saturation → Threshold"
                        }
                    }

                    SectionPanel {
                        width: parent.width
                        title: "Tone"

                        Text {
                            width: parent.width
                            wrapMode: Text.WordWrap
                            color: "#bdbdbd"
                            font.pixelSize: 12
                            text: "Global tonal controls similar to a Develop panel."
                        }

                        SliderRow {
                            id: brightnessSlider
                            label: "Brightness"
                            helper: "Shifts all RGB channels up or down."
                            slider.from: -1.0
                            slider.to: 1.0
                            slider.value: 0.0
                        }

                        SliderRow {
                            id: contrastSlider
                            label: "Contrast"
                            helper: "Expands or compresses values around middle gray."
                            slider.from: 0.0
                            slider.to: 2.0
                            slider.value: 1.0
                        }
                    }

                    SectionPanel {
                        width: parent.width
                        title: "Color"

                        Text {
                            width: parent.width
                            wrapMode: Text.WordWrap
                            color: "#bdbdbd"
                            font.pixelSize: 12
                            text: "Color styling controls for monochrome, saturation, and inversion."
                        }

                        SliderRow {
                            id: grayscaleSlider
                            label: "Black & White Amount"
                            helper: "Blends toward a grayscale version."
                            slider.from: 0.0
                            slider.to: 1.0
                            slider.value: 0.0
                        }

                        SliderRow {
                            id: saturationSlider
                            label: "Saturation"
                            helper: "0 = grayscale, 1 = normal color, >1 = boosted color."
                            slider.from: 0.0
                            slider.to: 2.0
                            slider.value: 1.0
                        }

                        SliderRow {
                            id: invertSlider
                            label: "Invert Amount"
                            helper: "Blends toward the inverted image."
                            slider.from: 0.0
                            slider.to: 1.0
                            slider.value: 0.0
                        }
                    }

                    SectionPanel {
                        width: parent.width
                        title: "Effects"

                        Text {
                            width: parent.width
                            wrapMode: Text.WordWrap
                            color: "#bdbdbd"
                            font.pixelSize: 12
                            text: "Creative effects and stylization controls."
                        }

                        SliderRow {
                            id: blurAmountSlider
                            label: "Blur Amount"
                            helper: "How strongly to mix the blur result back over the source."
                            slider.from: 0.0
                            slider.to: 1.0
                            slider.value: 0.0
                        }

                        SliderRow {
                            id: blurRadiusSlider
                            label: "Blur Radius"
                            helper: "How far away neighboring texels are sampled. Larger values create a more obvious blur."
                            slider.from: 0.0
                            slider.to: 12.0
                            slider.value: 1.0
                        }

                        SliderRow {
                            id: thresholdAmountSlider
                            label: "Threshold Amount"
                            helper: "How strongly to apply black/white thresholding based on brightness."
                            slider.from: 0.0
                            slider.to: 1.0
                            slider.value: 0.0
                        }

                        SliderRow {
                            id: thresholdCutoffSlider
                            label: "Threshold Cutoff"
                            helper: "The brightness boundary used to decide whether a pixel becomes black or white."
                            slider.from: 0.0
                            slider.to: 1.0
                            slider.value: 0.5
                        }
                    }

                    SectionPanel {
                        width: parent.width
                        title: "Masking"

                        Text {
                            width: parent.width
                            wrapMode: Text.WordWrap
                            color: "#bdbdbd"
                            font.pixelSize: 12
                            text: "Simple stack opacity control. This blends the full processed stack back over the original image."
                        }

                        SliderRow {
                            id: effectOpacitySlider
                            label: "Effect Opacity"
                            helper: "0 = original image, 1 = full processed stack."
                            slider.from: 0.0
                            slider.to: 1.0
                            slider.value: 1.0
                        }
                    }
                }
            }
        }
    }
}