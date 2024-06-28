#!/bin/bash

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

PROJECT_FILE="game/client/client_hl2mp.vcxproj.user"

# Create the file, remove it if it already exists
if [ -f $PROJECT_FILE ]; then
    rm $PROJECT_FILE
fi

cat <<EOL > $PROJECT_FILE
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="Current" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
    <PropertyGroup Condition="'\$(Configuration)|\$(Platform)'=='Debug|Win32'">
        <LocalDebuggerCommand>$STEAM_SDK_DIR\\hl2.exe</LocalDebuggerCommand>
        <LocalDebuggerCommandArguments>-allowdebug -dev -sw -game "$STEAM_MOD_DIR"</LocalDebuggerCommandArguments>
        <LocalDebuggerWorkingDirectory>$STEAM_SDK_DIR</LocalDebuggerWorkingDirectory>
        <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
    </PropertyGroup>
</Project>
EOL

# For whatever reason Valve decided to not allow us to configure dependencies in *.vpc files (am I wrong?)
# For this reason we modify the solution file to add the dependencies to Lua and Luasocket to the client
# and server projects
sln_file="experiment.sln"
echo "Modifying the solution file $sln_file..."

# Add dependencies to "Client (HL2MP)"
sed -i '/Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "Client (HL2MP)"/, /EndProjectSection/ {
    /EndProjectSection/ i\
\t\t{11C4CA93-C3BB-5EF6-0C85-700D6B69A2F6} = {11C4CA93-C3BB-5EF6-0C85-700D6B69A2F6}\
\t\t{950C58AA-39F0-9CA2-8BB5-1AD6B8011443} = {950C58AA-39F0-9CA2-8BB5-1AD6B8011443}
}' "$sln_file"

# Add dependencies to "Server (HL2MP)"
sed -i '/Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "Server (HL2MP)"/, /EndProjectSection/ {
    /EndProjectSection/ i\
\t\t{11C4CA93-C3BB-5EF6-0C85-700D6B69A2F6} = {11C4CA93-C3BB-5EF6-0C85-700D6B69A2F6}\
\t\t{950C58AA-39F0-9CA2-8BB5-1AD6B8011443} = {950C58AA-39F0-9CA2-8BB5-1AD6B8011443}
}' "$sln_file"

# Make luasocket depend on Lua:
sed -i '/Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "luasocket"/, /EndProject/ {
    /EndProject/ i\
\tProjectSection(ProjectDependencies) = postProject\
\t\t{11C4CA93-C3BB-5EF6-0C85-700D6B69A2F6} = {11C4CA93-C3BB-5EF6-0C85-700D6B69A2F6}\
\tEndProjectSection
}' "$sln_file"
