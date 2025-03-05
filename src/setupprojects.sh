#!/bin/bash

# Force to be relative to the script
cd "$(dirname "$0")"

IS_INITIAL_SETUP=false

if [[ $* == *--init* ]]
then
    IS_INITIAL_SETUP=true
fi

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

if [ -z "$STEAM_MOD_DIR" ]; then
    echo "STEAM_MOD_DIR is not set in the .env file!"
    exit 1
fi

if [ -z "$STEAM_SDK_DIR" ]; then
    echo "STEAM_SDK_DIR is not set in the .env file!"
    exit 1
fi

PROJECT_FILE="game/client/client_win64_experiment.vcxproj.user"

# Create the file, remove it if it already exists
if [ -f $PROJECT_FILE ]; then
    rm $PROJECT_FILE
fi

cat <<EOL > $PROJECT_FILE
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="Current" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
    <PropertyGroup Condition="'\$(Configuration)|\$(Platform)'=='Release|x64' or '\$(Configuration)|\$(Platform)'=='Debug|x64'">
        <LocalDebuggerCommand>\$(SolutionDir)..\\game\\experiment_win64.exe</LocalDebuggerCommand>
        <LocalDebuggerCommandArguments>-allowdebug -dev -sw -game "$STEAM_MOD_DIR"</LocalDebuggerCommandArguments>
        <LocalDebuggerWorkingDirectory>..\..\..\game</LocalDebuggerWorkingDirectory>
        <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
    </PropertyGroup>
</Project>
EOL

LAUNCHER_PROJECT_FILE="launcher_main/launcher_main_win64_experiment.vcxproj.user"

if [ -f $LAUNCHER_PROJECT_FILE ]; then
    rm $LAUNCHER_PROJECT_FILE
fi

cat <<EOL > $LAUNCHER_PROJECT_FILE
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="Current" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
    <PropertyGroup Condition="'\$(Configuration)|\$(Platform)'=='Release|x64' or '\$(Configuration)|\$(Platform)'=='Debug|x64'">
        <LocalDebuggerWorkingDirectory>\$(SolutionDir)/../game</LocalDebuggerWorkingDirectory>
        <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
    </PropertyGroup>
</Project>
EOL

if [ "$IS_INITIAL_SETUP" = true ]; then
    echo "Initial setup detected, copying the source SDK files..."
    # Copy the source SDK files over to the game directory
    ./../tools/prepare-standalone-game.sh
fi
