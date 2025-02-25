# Distributing your game

??? warning "Work in Progress"

    The documentation below currently describes how we want to implement the
    distribution of our game. As of right now it is not yet implemented.

## Overview

Broadly speaking, distributing your game involves the following steps:

1. Register your game on [experiment.games](https://experiment.games).

2. Download the game configuration scripts to your game's directory.

3. Compress your game.

4. Upload the zip file to [experiment.games](https://experiment.games).

5. Share the link to your game with your players.

## Registering your game

We envision Experiment as a platform for developers to create and distribute
their games. To this end, we will provide a web interface for developers to
register their games.

Registration involves providing some basic information about your game, such as
its name, description, and the game's directory. Furthermore you describe the
game directory name.

## Downloading the game configuration file

After registering your game, you will be able to download configuration scripts
from the Experiment website. These scripts will be placed next to `hl2.exe` in your
game's directory.

The following scripts will be available:

- `Start <your_game>.bat`:
    This script will ensure your game is started with the correct parameters.
    It will call the `hl2.exe` with `-steam -game experiment -config game.json`.

    *Note: we eventually hope to create a generic `Start <your_game>.exe` that
    is signed (so it doesn't trigger Windows Defender) and can be customized
    with your game's icon.*

- `game.json`:
    This file contains metadata about your game, like what the path is to the
    experiment.games API and update server.
    This will help the game to automatically update itself when you upload a new
    version to the website.

    Experiment checks for the `game.json` file in the game's directory and reads
    the `update_url` field to determine where to look for updates.

    ```json
    {
        "update_url": "https://api.experiment.games/v1/games/<your_game_id>/update"
    }
    ```

## Compressing your game

Seeing how you've developed your game with Lua, all the game files are already
inside the script and asset directories, like: `experiment/gamemodes`, 
`experiment/addons`, `experiment/materials`, etc.

To distribute your game, you will need to compress the folder above `experiment`
into a zip file. Meaning the zip file directly contains `hl2.exe`,
`Start <your_game>.bat` and `game.json`, with the `experiment` folder next to
them.

## Uploading your game

After compressing your game, you can upload the zip file to the Experiment
website. You will be able to upload new versions of your game at any time.

## Sharing your game

After uploading your game, you will receive a link to your game's page on the
Experiment website. You can share this link with your players, who will be able
to download and play your game.

## Not using the Experiment platform for distribution

We hope that you will use the Experiment platform to distribute your game. We
will charge a fee for hosting your game and keeping the platform up-to-date.

However, if you prefer to distribute your game yourself, you can still use the
same configuration scripts to distribute your game for free.
The update URL in the `game.json` file can point to your own server, and you
can create and distribute the configuration scripts with your game yourself.
