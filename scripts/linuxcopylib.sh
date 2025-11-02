#!/bin/bash
# linuxcopylib.sh to copy all the dependencies of TheCalculater to a directory

TARGET="TheCalculater"
DEST_DIR="."
QT_ARCH=linux_gcc_64
QT_PLUGIN_DIR="$QT_ROOT_DIR/plugins"

# NOTE: Manually add the plugins you need
QT_PLUGINS=(
    "platforms/libqxcb.so"
    "platforms/libqwayland.so"
    "imageformats/libqico.so"
    "imageformats/libqsvg.so"
    "imageformats/libqjpeg.so"
)

LIB_BLACKLIST=(
    "^/lib"
    "^/lib64" 
    "^/usr/lib"
    "^/usr/lib64"
    "^/usr/local/lib"
    "libTheCalculaterCommon.so"
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