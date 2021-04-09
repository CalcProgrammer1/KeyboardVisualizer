#!/bin/bash

#-----------------------------------------------------------------------#
# Keyboard Visualizer AppImage Build Script                             #
#-----------------------------------------------------------------------#

set -x
set -e

#-----------------------------------------------------------------------#
# Build in a temporary directory to keep the system clean               #
# Use RAM disk if possible (if available and not building on a CI       #
# system like Travis)                                                   #
#-----------------------------------------------------------------------#
if [ "$CI" == "" ] && [ -d /dev/shm ]; then
    TEMP_BASE=/dev/shm
else
    TEMP_BASE=/tmp
fi

BUILD_DIR=$(mktemp -d -p "$TEMP_BASE" appimage-build-XXXXXX)

#-----------------------------------------------------------------------#
# Make sure to clean up build dir, even if errors occur                 #
#-----------------------------------------------------------------------#
cleanup () {
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
    fi
}
trap cleanup EXIT

#-----------------------------------------------------------------------#
# Store repo root as variable                                           #
#-----------------------------------------------------------------------#
REPO_ROOT=$(readlink -f $(dirname $(dirname $0)))
OLD_CWD=$(readlink -f .)

#-----------------------------------------------------------------------#
# Switch to build dir                                                   #
#-----------------------------------------------------------------------#
pushd "$BUILD_DIR"

#-----------------------------------------------------------------------#
# Configure build files with qmake                                      #
# we need to explicitly set the install prefix, as qmake's default is   #
# /usr/local for some reason...                                         #
#-----------------------------------------------------------------------#
qmake "$REPO_ROOT"

#-----------------------------------------------------------------------#
# Build project and install files into AppDir                           #
#-----------------------------------------------------------------------#
make -j$(nproc)
make install INSTALL_ROOT=AppDir

#-----------------------------------------------------------------------#
# Make them executable                                                  #
#-----------------------------------------------------------------------#
chmod +x "$REPO_ROOT"/OpenRGB/scripts/tools/linuxdeploy*.AppImage

#-----------------------------------------------------------------------#
# Make sure Qt plugin finds QML sources so it can deploy the imported   #
# files                                                                 #
export QML_SOURCES_PATHS="$REPO_ROOT"/src

"$REPO_ROOT"/OpenRGB/scripts/tools/linuxdeploy-x86_64.AppImage --appimage-extract-and-run --appdir AppDir -e KeyboardVisualizer -i "$REPO_ROOT"/KeyboardVisualizerQT/KeyboardVisualizer.png -d "$REPO_ROOT"/KeyboardVisualizerQT/KeyboardVisualizer.desktop 
"$REPO_ROOT"/OpenRGB/scripts/tools/linuxdeploy-plugin-qt-x86_64.AppImage --appimage-extract-and-run --appdir AppDir
"$REPO_ROOT"/OpenRGB/scripts/tools/linuxdeploy-x86_64.AppImage --appimage-extract-and-run --appdir AppDir --output appimage

#-----------------------------------------------------------------------#
# Move built AppImage back into original CWD                            #
#-----------------------------------------------------------------------#
mv Keyboard_Visualizer*.AppImage "$OLD_CWD"

