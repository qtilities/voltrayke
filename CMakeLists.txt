cmake_minimum_required(VERSION 3.5)

project(VolTrayke VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)
find_package(ALSA)
find_package(PulseAudio)

set(TS_FILES resources/translations/voltrayke_it_IT.ts)

set(SOURCES
    src/audio/device.hpp
    src/audio/device.cpp
    src/audio/engine.hpp
    src/audio/engine.cpp
    src/audio/engineid.hpp
    src/ui/dialogabout.hpp
    src/ui/dialogabout.cpp
    src/ui/dialogabout.ui
    src/ui/dialogprefs.hpp
    src/ui/dialogprefs.cpp
    src/ui/dialogprefs.ui
    src/ui/menuvolume.hpp
    src/ui/menuvolume.cpp
    src/settings.hpp
    src/settings.cpp
    src/application.hpp
    src/application.cpp
    resources/resources.qrc
    ${TS_FILES}
)
set(LIBRARIES Qt${QT_VERSION_MAJOR}::Widgets)

if(ALSA_FOUND)
    add_definitions(-DUSE_ALSA)
    set(SOURCES ${SOURCES}
        src/audio/engine/alsa.hpp
        src/audio/engine/alsa.cpp
        src/audio/device/alsa.hpp
        src/audio/device/alsa.cpp
    )
    set(LIBRARIES ${LIBRARIES} ${ALSA_LIBRARIES})
endif()

if(PULSEAUDIO_FOUND)
    add_definitions(-DUSE_PULSEAUDIO)
    include_directories(${PULSEAUDIO_INCLUDE_DIR})
    set(SOURCES ${SOURCES}
        src/audio/engine/pulseaudio.hpp
        src/audio/engine/pulseaudio.cpp
    )
    set(LIBRARIES ${LIBRARIES} ${PULSEAUDIO_LIBRARY})
endif()

#if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
#    qt_add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
#    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
#else()
    add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
#endif()

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME voltrayke)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "src")
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${LIBRARIES})

if (UNIX AND NOT APPLE)
    include (GNUInstallDirs)
    install (TARGETS ${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_BINDIR})
    install (FILES resources/icons/${CMAKE_PROJECT_NAME}.svg
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)
    install (FILES resources/linux/${CMAKE_PROJECT_NAME}.appdata.xml
        DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)
    install (FILES resources/linux/${CMAKE_PROJECT_NAME}.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
#   install (FILES ${QM_FILES}
#       DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${CMAKE_PROJECT_NAME}/translations)
endif()
