cmake_minimum_required(VERSION 3.16)

project(QTSimpleImageProc VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick ShaderTools)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appQTSimpleImageProc
    main.cpp
)

qt_add_qml_module(appQTSimpleImageProc
    URI QTSimpleImageProc
    VERSION 1.0
    QML_FILES
        Main.qml
)

qt_add_shaders(appQTSimpleImageProc "shaders"
    PREFIX "/"
    FILES
        shaders/basic.frag
)

target_link_libraries(appQTSimpleImageProc
    PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS appQTSimpleImageProc
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)