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

PROJECT_FILE="game/client/client_experiment.vcxproj.user"

# Create the file, remove it if it already exists
if [ -f $PROJECT_FILE ]; then
    rm $PROJECT_FILE
fi

cat <<EOL > $PROJECT_FILE
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="Current" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
    <PropertyGroup Condition="'\$(Configuration)|\$(Platform)'=='Debug|Win32'">
        <LocalDebuggerCommand>\$(SolutionDir)..\\game\\hl2.exe</LocalDebuggerCommand>
        <LocalDebuggerCommandArguments>-allowdebug -dev -sw -game "$STEAM_MOD_DIR"</LocalDebuggerCommandArguments>
        <LocalDebuggerWorkingDirectory>$STEAM_SDK_DIR</LocalDebuggerWorkingDirectory>
        <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
    </PropertyGroup>
</Project>
EOL

# For whatever reason Valve decided to not allow us to configure dependencies in *.vpc files. For
# this reason we modify the solution file to add the dependencies to Lua and Luasocket to the client
# and server projects
# TODO: Please someone tell me I am wrong and there is a better way to do this
sln_file="experiment.sln"
echo "Modifying the solution file $sln_file..."

function addDependenciesToProject {
    local project_name=$1
    local dependency_guid=$2

    fromProject_to_EndProjectSection=$(sed -n "/Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"$project_name\"/, /EndProject/p" "$sln_file")
    if [[ $fromProject_to_EndProjectSection == *"$dependency_guid"* ]]; then
        echo "Dependency $dependency_guid already added to the project $project_name (ignoring)"
    else
        if [[ $fromProject_to_EndProjectSection != *"EndProjectSection"* ]]; then
            echo "Adding ProjectSection(ProjectDependencies) to the project $project_name"
            sed -i '/Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"'"$project_name"'\"/, /EndProject/ {
                /EndProject/ i\
\tProjectSection(ProjectDependencies) = postProject\
\tEndProjectSection
            }' "$sln_file"
        fi

        echo "Adding dependency $dependency_guid to the project $project_name"
        sed -i '/Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"'"$project_name"'\"/, /EndProjectSection/ {
            /EndProjectSection/ i\
\t\t{'"$dependency_guid"'} = {'"$dependency_guid"'}
        }' "$sln_file"
    fi
}

addDependenciesToProject "Client (Experiment)" "950C58AA-39F0-9CA2-8BB5-1AD6B8011443" # luasocket
addDependenciesToProject "Client (Experiment)" "11C4CA93-C3BB-5EF6-0C85-700D6B69A2F6" # lua
addDependenciesToProject "Client (Experiment)" "378EBA0C-3BA5-1CB2-6A4B-13E100D0686B" # libpng

addDependenciesToProject "Server (Experiment)" "950C58AA-39F0-9CA2-8BB5-1AD6B8011443" # luasocket
addDependenciesToProject "Server (Experiment)" "11C4CA93-C3BB-5EF6-0C85-700D6B69A2F6" # lua
addDependenciesToProject "Server (Experiment)" "378EBA0C-3BA5-1CB2-6A4B-13E100D0686B" # libpng

addDependenciesToProject "luasocket" "11C4CA93-C3BB-5EF6-0C85-700D6B69A2F6" # lua

if [ "$IS_INITIAL_SETUP" = true ]; then
    echo "Initial setup detected, copying the source SDK files..."
    # Copy the source SDK files over to the game directory
    ./../tools/prepare-standalone-game.sh
fi
