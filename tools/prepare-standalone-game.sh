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

# List of files and directories to copy
files=(
    "bin"
    "hl2"
    "hl2mp"
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
