#!/bin/bash

#
# This script will use the `STEAM_SDK_DIR` you set in the `.env`
# file to copy `hl2.exe` and other required content to the
# ../game/ directory.
# This will make the game a standalone game that can be run using
# the `experiment.bat` file inside that same directory.
#
# To run this you can use this from the root of the project:
#
#           ./tools/prepare-standalone-game.sh
#

# ensure we are relative to the script
cd "$(dirname "$0")"

if [ -f ../.env ]; then
    while IFS= read -r line; do
        if [[ $line == *"="* && $line != "#"* ]]; then
            eval "export $line"
        fi
    done <../.env
else
    echo "The .env file does not exist!"
    exit 1
fi

if [ -z "$STEAM_SDK_DIR" ]; then
    echo "STEAM_SDK_DIR is not set in the .env file!"
    exit 1
fi

target_dir="../game"

if [ ! -d "$target_dir" ]; then
    echo "The target directory $target_dir does not exist!"
    exit 1
fi

# List of files and directories to copy as is
files=(
    "bin"
    "platform"
    "thirdpartylegalnotices.txt"
    "hl2.exe"
)

# Before we proceed we will delete hl2.exe, or else the copy will fail
# when it tries to copy the directory with the same name (hl2)
if [ -e "$target_dir/hl2.exe" ]; then
    echo "Removing $target_dir/hl2.exe"
    rm "$target_dir/hl2.exe"
fi

for file in "${files[@]}"; do
    if [ -e "$STEAM_SDK_DIR/$file" ]; then
        echo "Copying $file to $target_dir"
        cp -r "$STEAM_SDK_DIR/$file" "$target_dir"
    else
        echo "The file $file does not exist in $STEAM_SDK_DIR"
    fi
done

# List of directories to merge into one 'engine' directory (overwriting existing files)
dirs=(
    "hl2mp"
    "hl2"
)

# Blocklist for files/directories in hl2(mp) not to include from the hl2 and hl2mp directories
blocklist=(
    "media" # No need for the valve intro
    "sound" # No need for the sound.cache
    "gameinfo.txt"
    "cfg"
    "custom"
    "download"
    "serverconfig.vdf"
    "steam.inf"
)

for dir in "${dirs[@]}"; do
    if [ -d "$STEAM_SDK_DIR/$dir" ]; then
        echo "Merging $dir to $target_dir/engine"
        mkdir -p "$target_dir/engine"
        cp -r "$STEAM_SDK_DIR/$dir"/* "$target_dir/engine"
        for block in "${blocklist[@]}"; do
            if [ -e "$target_dir/engine/$block" ]; then
                echo "Removing $target_dir/engine/$block"
                rm -r "$target_dir/engine/$block"
            fi
        done
    else
        echo "The directory $dir does not exist in $STEAM_SDK_DIR"
    fi
done
