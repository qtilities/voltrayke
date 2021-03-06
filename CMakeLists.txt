cmake_minimum_required(VERSION 3.5)

project(VolTrayke LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)

set(TS_FILES resources/translations/voltrayke_it_IT.ts)
set(RESOURCE_FILES resources/resources.qrc)
set(PROJECT_SOURCES
    src/ui/dialogprefs.cpp
    src/ui/dialogprefs.hpp
    src/ui/dialogprefs.ui
    src/ui/menuactions.cpp
    src/ui/menuactions.hpp
    src/ui/menuvolume.cpp
    src/ui/menuvolume.hpp
    src/ui/menuvolume.ui
    src/voltrayke.cpp
    src/voltrayke.hpp
    ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
        ${RESOURCE_FILES}
    )
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED
            ${PROJECT_SOURCES}
            ${RESOURCE_FILES}
        )
    else()
        add_executable(${PROJECT_NAME}
            ${PROJECT_SOURCES}
            ${RESOURCE_FILES}
        )
    endif()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME voltrayke)

source_group ("Resource Files" FILES ${RESOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE "src")
target_link_libraries(VolTrayke PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
)
if (UNIX AND NOT APPLE)
    include (GNUInstallDirs)
    install (TARGETS ${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_BINDIR})
    install (FILES resources/icons/${PROJECT_NAME}.svg
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)
    install (FILES resources/linux/${PROJECT_NAME}.appdata.xml
        DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)
    install (FILES resources/linux/${PROJECT_NAME}.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
#   install (FILES ${QM_FILES}
#       DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/translations)
endif()
