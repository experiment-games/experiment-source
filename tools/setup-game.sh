#!/bin/bash

# Force to be relative to the script
cd "$(dirname "$0")"

# Without the settings.scr file, the default settings from
# HL2DM would be used. We have to ensure a settings.scr file
# is present in the game/experiment/cfg directory.
cp ../game/experiment/cfg/settings_default.scr ../game/experiment/cfg/settings.scr
