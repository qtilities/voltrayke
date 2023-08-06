cmake_minimum_required(VERSION 3.15)
project(VolTrayke
    VERSION 0.2.0
    LANGUAGES C CXX
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#===============================================================================
# Qt
#===============================================================================
option(PROJECT_TRANSLATIONS_UPDATE "Update source translations [default: OFF]" OFF)
set(PROJECT_QT_VERSION 5 CACHE STRING "Qt version to use [default: 5]")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(QT NAMES Qt${PROJECT_QT_VERSION})
find_package(Qt${QT_VERSION_MAJOR} REQUIRED LinguistTools Widgets)
find_package(Qtilitools REQUIRED)
#===============================================================================
# Dependencies
#===============================================================================
option(PROJECT_USE_ALSA "Whether to use ALSA audio engine [default: ON]" ON)
option(PROJECT_USE_PULSEAUDIO "Whether to use PulseAudio engine [default: ON]" ON)
if(PROJECT_USE_ALSA)
    find_package(ALSA REQUIRED)
endif()
if(PROJECT_USE_PULSEAUDIO)
    find_package(PulseAudio REQUIRED)
endif()
#===============================================================================
# Project files
#===============================================================================
set(PROJECT_SOURCES
    src/application.hpp
    src/application.cpp
    src/audio/device.hpp
    src/audio/device.cpp
    src/audio/engine.hpp
    src/audio/engine.cpp
    src/audio/engineid.hpp
    src/dialogabout.hpp
    src/dialogabout.cpp
    src/dialogprefs.hpp
    src/dialogprefs.cpp
    src/menuvolume.hpp
    src/menuvolume.cpp
    src/qtilities.hpp
    src/settings.hpp
    src/settings.cpp
)
if(PROJECT_USE_ALSA)
    list(APPEND PROJECT_SOURCES
        src/audio/engine/alsa.hpp
        src/audio/engine/alsa.cpp
        src/audio/device/alsa.hpp
        src/audio/device/alsa.cpp
    )
endif()
if(PROJECT_USE_PULSEAUDIO)
    list(APPEND PROJECT_SOURCES
        src/audio/engine/pulseaudio.hpp
        src/audio/engine/pulseaudio.cpp
    )
endif()
set(PROJECT_UI_FILES
    src/dialogabout.ui
    src/dialogprefs.ui
)
set(PROJECT_OTHER_FILES
    .github/workflows/build.yml
    .clang-format
    .editorconfig
    .gitattributes
    .gitignore
    README.md
)
source_group("Other Files" FILES ${PROJECT_OTHER_FILES})
source_group("UI Files" FILES ${PROJECT_UI_FILES})
#===============================================================================
# Resources
#===============================================================================
include(Config.cmake)
include(QtAppResources)
#===============================================================================
# Application executable
#===============================================================================
set(PROJECT_ALL_FILES
    ${PROJECT_DESKTOP_FILES}
    ${PROJECT_RESOURCES}
    ${PROJECT_SOURCES}
    ${PROJECT_OTHER_FILES}
    ${PROJECT_QM_FILES}
    ${PROJECT_TRANSLATION_SOURCES}
    ${PROJECT_UI_FILES}
)
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION ${PROJECT_ALL_FILES})
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED ${PROJECT_ALL_FILES})
    else()
        add_executable(${PROJECT_NAME} ${PROJECT_ALL_FILES})
    endif()
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_ID})

target_include_directories(${PROJECT_NAME} PRIVATE
    "src"
    ${ALSA_INCLUDE_DIR}
    ${PULSEAUDIO_INCLUDE_DIR}
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Widgets
    ${ALSA_LIBRARIES}
    ${PULSEAUDIO_LIBRARY}
)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_ID}")

target_compile_definitions(${PROJECT_NAME} PRIVATE
    APPLICATION_DISPLAY_NAME="${PROJECT_NAME}"
    APPLICATION_NAME="${PROJECT_ID}"
    ORGANIZATION_NAME="${PROJECT_ORGANIZATION_NAME}"
    ORGANIZATION_DOMAIN="${PROJECT_ORGANIZATION_URL}"
    PROJECT_DATA_DIR="${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_ID}"
)
if(PROJECT_USE_ALSA)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_ALSA=1)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_ALSA=0)
endif()
if(PROJECT_USE_PULSEAUDIO)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_PULSEAUDIO=1)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_PULSEAUDIO=0)
endif()
#===============================================================================
# Install application
#===============================================================================
if (UNIX AND NOT APPLE)
    install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
#===============================================================================
# Project information
#===============================================================================
message(STATUS "
Project name:                     ${PROJECT_NAME}
AppStream ID:                     ${PROJECT_APPSTREAM_ID}
Version:                          ${PROJECT_VERSION}
Qt version:                       ${QT_VERSION}
Build type:                       ${CMAKE_BUILD_TYPE}
Install prefix:                   ${CMAKE_INSTALL_PREFIX}
Update translations before build: ${PROJECT_TRANSLATIONS_UPDATE}

CXX Debug flags:                  ${CMAKE_CXX_FLAGS_DEBUG}
CXX Release flags:                ${CMAKE_CXX_FLAGS_RELEASE}
CXX MinSize flags:                ${CMAKE_CXX_FLAGS_MINSIZEREL}
CXX RelWithDebInfo flags:         ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}
")

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
