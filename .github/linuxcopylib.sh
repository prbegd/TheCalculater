#!/bin/bash
# linuxcopylib.sh to copy all the dependencies of TheCalculater to a directory
#
# Copyright © 2025 Cai Yaoxing
# SPDX-License-Identifier: GPL-3.0-only
# This file is part of TheCalculater.
# See the file LICENSE in the project root or go to
# <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.

TARGET="TheCalculater"
DEST_DIR="."
QT_ARCH=linux_gcc_64
QT_PLUGIN_DIR="$QT_ROOT_DIR/plugins"

ls $QT_PLUGIN_DIR/platforms

# NOTE: Manually add the plugins you need
QT_PLUGINS=(
    "platforms/libqxcb.so"
    "platforms/libqwayland.so"
    "imageformats/libqico.so"
    "imageformats/libqsvg.so"
    "imageformats/libqjpeg.so"
    "wayland-shell-integration/libfullscreen-shell-v1.so"
    "wayland-shell-integration/libivi-shell.so"
    "wayland-shell-integration/libqt-shell.so"
    "wayland-shell-integration/libwl-shell-plugin.so"
    "wayland-shell-integration/libxdg-shell.so"
    "wayland-decoration-client/libbradient.so"
)

LIB_BLACKLIST=(
    "^/lib"
    "^/lib64" 
    "^/usr/lib"
    "^/usr/lib64"
    "^/usr/local/lib"
    "libTheCalculaterCommon.so"
    "libTheCalculaterQtBridge.so"
    "libThirdPartyModulesMock.so"
)

copylibfor() {
    ldd "$1" | grep "=> /" | awk '{print $3}' | while read -r lib; do
        is_in_blacklist=0
        for lib_blacklist in "${LIB_BLACKLIST[@]}"; do
            if [[ "$lib" =~ $lib_blacklist ]]; then
                is_in_blacklist=1
                break
            fi
        done
        
        if [[ $is_in_blacklist -eq 0 && -f "$lib" ]]; then
            echo "Copying library: $lib for $1"
            cp "$lib" "$DEST_DIR"
        fi
    done
}

copylibfor $TARGET

for plugin in "${QT_PLUGINS[@]}"; do
    pluginPath="$QT_PLUGIN_DIR/$plugin"
    echo "Copying plugin: $plugin for $TARGET"
    mkdir -p "$DEST_DIR/$(dirname $plugin)"
    cp "$pluginPath" "$DEST_DIR/$plugin"
    copylibfor "$pluginPath"
done

echo "Dependencies copied to $DEST_DIR"