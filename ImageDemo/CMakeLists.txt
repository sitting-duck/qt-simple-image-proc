cmake_minimum_required(VERSION 3.21)
project(ImageDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    QuickWidgets
    Qml
    Concurrent
    ShaderTools
    Network
)

qt_standard_project_setup()

qt_add_executable(ImageDemoApp
    src/main.cpp
    src/MainWindow.h
    src/MainWindow.cpp
    src/EffectSettings.h
    src/EffectSettings.cpp
    src/PreviewController.h
    src/PreviewController.cpp
    src/PhotoSyncClient.h
    src/PhotoSyncClient.cpp
)

qt_add_qml_module(ImageDemoApp
    URI ImageDemo
    VERSION 1.0
    QML_FILES
        qml/PreviewView.qml
)

qt_add_shaders(ImageDemoApp "imagedemo_shaders"
    PREFIX
        "/shaders"
    BASE
        "shaders"
    FILES
        shaders/passthrough.vert
        shaders/blur.frag
)

target_link_libraries(ImageDemoApp PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::QuickWidgets
    Qt6::Qml
    Qt6::Concurrent
    Qt6::ShaderTools
    Qt6::Network
)