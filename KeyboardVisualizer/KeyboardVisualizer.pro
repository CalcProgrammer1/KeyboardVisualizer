#-------------------------------------------------
#
# Project created by QtCreator 2016-12-18T13:03:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KeyboardVisualizer
TEMPLATE = app


SOURCES += main.cpp \
    KeyboardVisDlg.cpp \
    chuck_fft.c \
    hsv.cpp \
    Visualizer.cpp \
    LEDStrip.cpp \
    serial_port.cpp \
    net_port.cpp \
    RazerChromaLinux.cpp

HEADERS  += \
    KeyboardVisDlg.h \
    Visualizer.h \
    VisualizerDefines.h \
    hsv.h \
    chuck_fft.h \
    LEDStrip.h \
    serial_port.h \
    RazerChromaLinux.h \
    net_port.h \
    CorsairCKBLinux.h

FORMS    += keyboardvisualizer.ui

LIBS      += -lopenal

DISTFILES +=

RESOURCES += \
    resources.qrc
