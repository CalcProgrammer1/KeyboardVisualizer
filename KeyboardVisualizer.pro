#-----------------------------------------------------------------------#
# Keyboard Visualizer 4.x QMake Project                                 #
#                                                                       #
#   Adam Honse (CalcProgrammer1)                        5/15/2020       #
#-----------------------------------------------------------------------#

#-----------------------------------------------------------------------#
# Qt Configuration                                                      #
#-----------------------------------------------------------------------#
QT +=                                                                   \
    core                                                                \
    gui                                                                 \

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#-----------------------------------------------------------------------#
# Application Configuration                                             #
#-----------------------------------------------------------------------#
VERSION     = 4.00
TARGET      = KeyboardVisualizer
TEMPLATE    = app

#-----------------------------------------------------------------------#
# Automatically generated build information                             #
#-----------------------------------------------------------------------#
win32:BUILDDATE = $$system(date /t)
unix:BUILDDATE  = $$system(date -R)
GIT_COMMIT_ID   = $$system(git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$_PRO_FILE_PWD_ rev-parse HEAD)
GIT_COMMIT_DATE = $$system(git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$_PRO_FILE_PWD_ show -s --format=%ci HEAD)
GIT_BRANCH      = $$system(git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$_PRO_FILE_PWD_ rev-parse --abbrev-ref HEAD)

DEFINES +=                                                              \
    VERSION_STRING=\\"\"\"$$VERSION\\"\"\"                              \
    BUILDDATE_STRING=\\"\"\"$$BUILDDATE\\"\"\"                          \
    GIT_COMMIT_ID=\\"\"\"$$GIT_COMMIT_ID\\"\"\"                         \
    GIT_COMMIT_DATE=\\"\"\"$$GIT_COMMIT_DATE\\"\"\"                     \
    GIT_BRANCH=\\"\"\"$$GIT_BRANCH\\"\"\"                               \

#-----------------------------------------------------------------------#
# Keyboard Visualizer                                                   #
#-----------------------------------------------------------------------#
HEADERS +=                                                              \
    KeyboardVisualizerCommon/chuck_fft.h                                \
    KeyboardVisualizerCommon/hsv.h                                      \
    KeyboardVisualizerCommon/Visualizer.h                               \
    KeyboardVisualizerCommon/VisualizerDefines.h                        \
    KeyboardVisualizerQT/KeyboardVisDlg.h                               \

SOURCES +=                                                              \
    KeyboardVisualizerCommon/hsv.cpp                                    \
    KeyboardVisualizerCommon/Visualizer.cpp                             \
    KeyboardVisualizerCommon/chuck_fft.c                                \
    KeyboardVisualizerQT/KeyboardVisDlg.cpp                             \
    KeyboardVisualizerQT/main.cpp                                       \

RESOURCES +=                                                            \
    KeyboardVisualizerQT/resources.qrc

FORMS +=                                                                \
    KeyboardVisualizerQT/keyboardvisualizer.ui

RC_ICONS +=                                                             \
    KeyboardVisualizerQT/KeyboardVisualizer.ico

DISTFILES +=                                                            \

#-----------------------------------------------------------------------#
# OpenRGB SDK                                                           #
#-----------------------------------------------------------------------#
INCLUDEPATH +=                                                          \
    OpenRGB/                                                            \
    OpenRGB/net_port/                                                   \
    OpenRGB/RGBController/                                              \

HEADERS +=                                                              \
    OpenRGB/NetworkClient.h                                             \
    OpenRGB/NetworkProtocol.h                                           \
    OpenRGB/net_port/net_port.h                                         \
    OpenRGB/RGBController/RGBController.h                               \
    OpenRGB/RGBController/RGBController_Network.h                       \

SOURCES +=                                                              \
    OpenRGB/NetworkClient.cpp                                           \
    OpenRGB/net_port/net_port.cpp                                       \
    OpenRGB/RGBController/RGBController.cpp                             \
    OpenRGB/RGBController/RGBController_Network.cpp                     \

#-----------------------------------------------------------------------#
# Windows-specific Configuration                                        #
#-----------------------------------------------------------------------#
win32:contains(QMAKE_TARGET.arch, x86_64) {
    LIBS +=                                                             \
        -lws2_32                                                        \
        -lole32                                                         \
}

win32:contains(QMAKE_TARGET.arch, x86) {
    LIBS +=                                                             \
        -lws2_32                                                        \
        -lole32                                                         \
}

win32:DEFINES -=                                                        \
    UNICODE

win32:DEFINES +=                                                        \
    _MBCS                                                               \
    WIN32                                                               \
    _CRT_SECURE_NO_WARNINGS                                             \
    _WINSOCK_DEPRECATED_NO_WARNINGS                                     \
    WIN32_LEAN_AND_MEAN

#-----------------------------------------------------------------------#
# Linux-specific Configuration                                          #
#-----------------------------------------------------------------------#
unix:!macx {
    LIBS += -lopenal
}

#-----------------------------------------------------------------------#
# MacOS-specific Configuration                                          #
#-----------------------------------------------------------------------#
macx: {
    LIBS += -framework OpenAL
}
