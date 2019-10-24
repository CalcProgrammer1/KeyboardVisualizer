#-------------------------------------------------
#
# Project created by QtCreator 2016-12-18T13:03:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KeyboardVisualizer
TEMPLATE = app

LIBS      += -lopenal -le131

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
    OpenAuraSDK/                                                        \
    OpenAuraSDK/i2c_smbus/                                              \
    OpenAuraSDK/net_port/                                               \
    OpenAuraSDK/serial_port/                                            \
    OpenAuraSDK/Controllers/AuraController/                             \
    OpenAuraSDK/Controllers/CorsairController/                          \
    OpenAuraSDK/Controllers/CorsairProController/                       \
    OpenAuraSDK/Controllers/HuePlusController/                          \
    OpenAuraSDK/Controllers/HyperXController/                           \
    OpenAuraSDK/Controllers/LEDStripController/                         \
    OpenAuraSDK/RGBController/                                          \
    OpenAuraSDK/qt/

SOURCES +=                                                              \
    KeyboardVisualizerCommon/hsv.cpp                                    \
    KeyboardVisualizerCommon/Visualizer.cpp                             \
    KeyboardVisualizerCommon/chuck_fft.c                                \
    KeyboardVisualizerQT/KeyboardVisDlg.cpp                             \
    KeyboardVisualizerQT/main.cpp                                       \
    KeyboardVisualizerCommon/UsbDevice.cpp                              \

# From OpenAuraSDK
SOURCES +=                                                                          \
    OpenAuraSDK/OpenAuraSDK.cpp                                                     \
    OpenAuraSDK/i2c_smbus/i2c_smbus.cpp                                             \
    OpenAuraSDK/i2c_smbus/i2c_smbus_linux.cpp                                       \
    OpenAuraSDK/net_port/net_port.cpp                                               \
    OpenAuraSDK/serial_port/serial_port.cpp                                         \
    OpenAuraSDK/Controllers/AuraController/AuraController.cpp                       \
    OpenAuraSDK/Controllers/AuraController/AuraControllerDetect.cpp                 \
    OpenAuraSDK/Controllers/CorsairController/CorsairController.cpp                 \
    OpenAuraSDK/Controllers/CorsairController/CorsairControllerDetect.cpp           \
    OpenAuraSDK/Controllers/CorsairProController/CorsairProController.cpp           \
    OpenAuraSDK/Controllers/CorsairProController/CorsairProControllerDetect.cpp     \
    OpenAuraSDK/Controllers/HuePlusController/HuePlusController.cpp                 \
    OpenAuraSDK/Controllers/HuePlusController/HuePlusControllerDetect.cpp           \
    OpenAuraSDK/Controllers/HyperXController/HyperXController.cpp                   \
    OpenAuraSDK/Controllers/HyperXController/HyperXControllerDetect.cpp             \
    OpenAuraSDK/Controllers/LEDStripController/LEDStripController.cpp               \
    OpenAuraSDK/Controllers/LEDStripController/LEDStripControllerDetect.cpp         \
    OpenAuraSDK/RGBController/E131ControllerDetect.cpp                              \
    OpenAuraSDK/RGBController/OpenRazerDetect.cpp                                   \
    OpenAuraSDK/RGBController/RGBController_Aura.cpp                                \
    OpenAuraSDK/RGBController/RGBController_Corsair.cpp                             \
    OpenAuraSDK/RGBController/RGBController_CorsairPro.cpp                          \
    OpenAuraSDK/RGBController/RGBController_E131.cpp                                \
    OpenAuraSDK/RGBController/RGBController_HuePlus.cpp                             \
    OpenAuraSDK/RGBController/RGBController_HyperX.cpp                              \
    OpenAuraSDK/RGBController/RGBController_LEDStrip.cpp                            \
    OpenAuraSDK/RGBController/RGBController_OpenRazer.cpp

HEADERS += \
    KeyboardVisualizerCommon/chuck_fft.h                              \
    KeyboardVisualizerCommon/hsv.h                                    \
    KeyboardVisualizerCommon/net_port.h                               \
    KeyboardVisualizerCommon/Visualizer.h                             \
    KeyboardVisualizerCommon/VisualizerDefines.h                      \
    KeyboardVisualizerQT/KeyboardVisDlg.h                             \
    KeyboardVisualizerCommon/UsbDevice.h                              \

# From OpenAuraSDK
HEADERS += \
    OpenAuraSDK/i2c_smbus/i2c_smbus.h                                               \
    OpenAuraSDK/i2c_smbus/i2c_smbus_linux.h                                         \
    OpenAuraSDK/net_port/net_port.h                                                 \
    OpenAuraSDK/serial_port/serial_port.h                                           \
    OpenAuraSDK/Controllers/AuraController/AuraController.h                         \
    OpenAuraSDK/Controllers/CorsairController/CorsairController.h                   \
    OpenAuraSDK/Controllers/CorsairProController/CorsairProController.h             \
    OpenAuraSDK/Controllers/HyperXController/HyperXController.h                     \
    OpenAuraSDK/Controllers/LEDStripController/LEDStripController.h                 \
    OpenAuraSDK/RGBController/RGBController.h                                       \
    OpenAuraSDK/RGBController/RGBController_Aura.h                                  \
    OpenAuraSDK/RGBController/RGBController_Corsair.h                               \
    OpenAuraSDK/RGBController/RGBController_CorsairPro.h                            \
    OpenAuraSDK/RGBController/RGBController_E131.h                                  \
    OpenAuraSDK/RGBController/RGBController_HyperX.h                                \
    OpenAuraSDK/RGBController/RGBController_OpenRazer.h

RESOURCES += \
    KeyboardVisualizerQT/resources.qrc

FORMS += \
    KeyboardVisualizerQT/keyboardvisualizer.ui
