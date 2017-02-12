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

HEADERS += \
    KeyboardVisualizerCommon/chuck_fft.h \
    KeyboardVisualizerCommon/hsv.h \
    KeyboardVisualizerCommon/LEDStrip.h \
    KeyboardVisualizerCommon/net_port.h \
    KeyboardVisualizerCommon/serial_port.h \
    KeyboardVisualizerCommon/Visualizer.h \
    KeyboardVisualizerCommon/VisualizerDefines.h \
    KeyboardVisualizerCommon/CorsairCKBLinux.h \
    KeyboardVisualizerCommon/RazerChromaLinux.h \
    KeyboardVisualizerQT/KeyboardVisDlg.h \
    KeyboardVisualizerCommon/UsbDevice.h \
    KeyboardVisualizerCommon/SteelSeriesGameSense.h \
    KeyboardVisualizerCommon/PoseidonZRGBKeyboard.h \
    KeyboardVisualizerCommon/MSIKeyboard.h

SOURCES += \
    KeyboardVisualizerCommon/hsv.cpp \
    KeyboardVisualizerCommon/LEDStrip.cpp \
    KeyboardVisualizerCommon/net_port.cpp \
    KeyboardVisualizerCommon/serial_port.cpp \
    KeyboardVisualizerCommon/Visualizer.cpp \
    KeyboardVisualizerCommon/chuck_fft.c \
    KeyboardVisualizerCommon/CorsairCKBLinux.cpp \
    KeyboardVisualizerCommon/RazerChromaLinux.cpp \
    KeyboardVisualizerQT/KeyboardVisDlg.cpp \
    KeyboardVisualizerQT/main.cpp \
    KeyboardVisualizerCommon/UsbDevice.cpp \
    KeyboardVisualizerCommon/SteelSeriesGameSense.cpp \
    KeyboardVisualizerCommon/PoseidonZRGBKeyboard.cpp \
    KeyboardVisualizerCommon/MSIKeyboard.cpp

RESOURCES += \
    KeyboardVisualizerQT/resources.qrc

FORMS += \
    KeyboardVisualizerQT/keyboardvisualizer.ui
