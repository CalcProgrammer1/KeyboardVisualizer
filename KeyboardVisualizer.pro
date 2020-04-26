#-------------------------------------------------
#
# Project created by QtCreator 2016-12-18T13:03:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KeyboardVisualizer
TEMPLATE = app

LIBS      += -lopenal

packagesExist(hidapi-libusb) {
  unix:LIBS += -lhidapi-libusb
} else {
  packagesExist(hidapi) {
    unix:LIBS += -lhidapi
  } else {
    unix:LIBS += -lhidapi-libusb
  }
}

DISTFILES +=

INCLUDEPATH +=                                                          \
    OpenRGB/                                                            \
    OpenRGB/dependencies/libe131/src/                                   \
    OpenRGB/net_port/                                                   \
    OpenRGB/RGBController/                                              \
    OpenRGB/qt/                                                         \

SOURCES +=                                                              \
    KeyboardVisualizerCommon/hsv.cpp                                    \
    KeyboardVisualizerCommon/Visualizer.cpp                             \
    KeyboardVisualizerCommon/chuck_fft.c                                \
    KeyboardVisualizerQT/KeyboardVisDlg.cpp                             \
    KeyboardVisualizerQT/main.cpp                                       \

# From OpenRGB
SOURCES +=                                                              \
    OpenRGB/NetworkClient.cpp                                           \
    OpenRGB/net_port/net_port.cpp                                       \
    OpenRGB/RGBController/RGBController.cpp                             \
    OpenRGB/RGBController/RGBController_Network.cpp                     \

HEADERS += \
    KeyboardVisualizerCommon/chuck_fft.h                                \
    KeyboardVisualizerCommon/hsv.h                                      \
    KeyboardVisualizerCommon/net_port.h                                 \
    KeyboardVisualizerCommon/Visualizer.h                               \
    KeyboardVisualizerCommon/VisualizerDefines.h                        \
    KeyboardVisualizerQT/KeyboardVisDlg.h                               \

# From OpenRGB
HEADERS += \
    OpenRGB/NetworkClient.h                                             \
    OpenRGB/NetworkProtocol.h                                           \
    OpenRGB/net_port/net_port.h                                         \
    OpenRGB/RGBController/RGBController.h                               \
    OpenRGB/RGBController/RGBController_Network.h                       \

RESOURCES += \
    KeyboardVisualizerQT/resources.qrc

FORMS += \
    KeyboardVisualizerQT/keyboardvisualizer.ui
