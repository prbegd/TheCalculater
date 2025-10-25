#!/bin/bash
# linuxcopylib.sh

TARGET=$1
DEST_DIR=$2

LIB_BLACKLIST=(
    "/lib"
    "/lib64" 
    "/usr/lib"
    "/usr/lib64"
    "/usr/local/lib"
    "libTheCalculaterCommon.so"
)

cp "$TARGET" "$DEST_DIR"

ldd "$TARGET" | grep "=> /" | awk '{print $3}' | while read -r lib; do
    is_in_blacklist=0
    for lib_blacklist in "${LIB_BLACKLIST[@]}"; do
        if [[ "$lib" =~ $lib_blacklist ]]; then
            is_in_blacklist=1
            break
        fi
    done
    
    if [[ $is_in_blacklist -eq 0 && -f "$lib" ]]; then
        echo "Copying: $lib"
        cp -v "$lib" "$DEST_DIR"
    fi
done

echo "Copied libraries to $DEST_DIR"