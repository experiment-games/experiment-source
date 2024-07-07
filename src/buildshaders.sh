#!/bin/bash

#
# Instructions:
#
# 1. Install https://strawberryperl.com/ for Windows
# 2. Open the cpan terminal: `cpan`
# 3. Run the initial setup: `o conf init`
# 4. Run: `install String::CRC32`
# 5. Ensure the .env file is in the root directory and is setup
#

# Ensure we are relative to where the script is
cd "$(dirname "$0")"

if [ -f ../.env ]; then
    while IFS= read -r line; do
        if [[ $line == *"="* && $line != "#"* ]]; then
            eval "export $line"
        fi
    done < ../.env
else
    echo "The .env file does not exist!"
    exit 1
fi

if [ -z "$STEAM_SDK_DIR" ]; then
    echo "STEAM_SDK_DIR is not set in the .env file!"
    exit 1
fi

if [ -z "$NMAKE_PATH" ]; then
    echo "NMAKE_PATH is not set in the .env file!"
    exit 1
fi

if [ -z "$PERL_PATH" ]; then
    echo "PERL_PATH is not set in the .env file!"
    exit 1
fi

#

cd materialsystem/stdshaders

GAMEDIR="../../../game/experiment"
SOURCEDIR="../.."

export SDKBINDIR="$STEAM_SDK_DIR/bin"
export GAMEDIR=$(cd "$GAMEDIR" && pwd)
export SOURCEDIR=$(cd "$SOURCEDIR" && pwd)

# Use dynamic shaders to build .inc files only
# dynamic_shaders=1

export NMAKEPATH="$NMAKE_PATH"
export PERLPATH="$PERL_PATH"

PERLPATHBASE=$(dirname "$PERLPATH")

export PATH="$PERLPATHBASE:$PATH"

TTEXE="../../devtools/bin/timeprecise.exe"
if [ ! -f "$TTEXE" ]; then
    TTEXE="date +%T"
fi

echo ~~~~~~ buildsdkshaders "$@" ~~~~~~
$TTEXE
tt_all_start=$?
tt_all_chkpt=$tt_start

BUILD_SHADER="./buildshaders.bat"
ARG_EXTRA=""

$BUILD_SHADER stdshader_dx9_20b -game "$GAMEDIR" -source "$SOURCEDIR"
$BUILD_SHADER stdshader_dx9_30 -game "$GAMEDIR" -source "$SOURCEDIR" -dx9_30 -force30

if [ "$dynamic_shaders" != "1" ]; then
  echo Finished full buildallshaders "$@"
  :
else
  echo Finished dynamic buildallshaders "$@"
  :
fi

