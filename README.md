# Experiment (Source Engine Mod)

Initially this repository mainly worked towards creating a Source Engine mod
called "Experiment". It is a multiplayer mod that is based on the Source SDK 2013.
&raquo; [🕹 Read more about Experiment in the Garry's Mod gamemode repository](https://github.com/luttje/experiment-redux)

However an alternative idea that arose during development is that Experiment could
be used as a platform for creating Source Engine mods. This would allow developers
to create their own mods based on the Source SDK 2013, but without writing C++.
Instead mods could be created with Lua scripting and other features not present
in the original Source Engine SDK.
&raquo; [💡 Read more about Experiment as a platform in the Developer Portal](https://luttje.github.io/experiment-source)

<div align="middle">

![Experiment Redux logo](./logo.png)

**🐕 It's a dog-eat-dog world out there, and these dogs have guns.**

</div>

## Requirements

- Source SDK 2013 Multiplayer Base (Installed through Steam)
- Visual Studio 2022 (or newer)
- [Visual Studio 2013 (for the Source SDK 2013 solution)](https://archive.org/details/en_visual_studio_community_2013_with_update_5_x86_dvd_6816332)

[&raquo; See the Source SDK 2013 documentation for more information.](https://developer.valvesoftware.com/wiki/Source_SDK_2013)

## Getting started

1. Go into the `src/` directory and generate the Visual Studio solution.

    ```bash
    cd src
    creategameprojects.bat
    ```

2. Copy the `.env.example` file to `.env`.

    ```bash
    cp .env.example .env
    ```

3. Fill the `.env` file with the described values

4. Next run the `setupprojects.sh` script to modify the Visual Studio project files and solution.

    ```bash
    ./setupprojects.sh --init
    ```

    _This sets up debugging and enforces the correct dependencies of the client & server projects._

5. Open the created `experiment.sln` solution in Visual Studio 2022 (or newer).

    > [!WARNING]
    > Despite building in Visual Studio 2022, the Source SDK 2013 solution requires Visual Studio 2013 to be installed.

6. Build the solution in `Release` mode.

7. After the build is complete make a symlink from the `game/experiment` directory to your `steamapps/sourcemods` directory. E.g:

    ```bash
    mklink /J "C:\Program Files (x86)\Steam\steamapps\sourcemods\experiment" "<path to this repo>\game\experiment"
    ```

8. To setup the game mod directory, run:

    ```bash
    ./tools/setup-game.sh
    ```

## Documentation

We build and present the Experiment Developer Documentation using [mkdocs-material](https://squidfunk.github.io/mkdocs-material/).

- [Read our conventions](./docs/general/conventions.md)

### Requirements

- Python 3.8 or newer

### Installation

1. Install the required Python packages.

    ```bash
    pip install -r requirements.txt
    ```

2. Start the development server.

    ```bash
    mkdocs serve --dirty
    ```

    _The `--dirty` flag is used to make live-reloading only rebuild the changed_
    _files, rather than the entire site. This is useful for faster development._

3. Open the documentation in your browser @ [http://localhost:8000/experiment-source/]

## Extras

<details>

<summary>Updating from Source SDK 2013</summary>

1. Switch to the `master` branch.

    ```bash
    git checkout master
    ```

2. Ensure that the upstream repository is added as a remote.

    ```bash
    git remote add upstream https://github.com/ValveSoftware/source-sdk-2013
    git remote set-url --push upstream DISABLE
    ```

3. Fetch the latest changes from the upstream repository.

    ```bash
    git fetch upstream
    ```

4. Merge the changes from the upstream repository into the `master` branch.

    ```bash
    git merge upstream/master
    ```

5. Resolve any merge conflicts, if necessary.

6. Push the changes to this forked repository.

    ```bash
    git push origin master
    ```

7. Switch to the `experiment-main` branch.

    ```bash
    git checkout experiment-main
    ```

8. Merge the changes from the `master` branch into the `experiment-main` branch.

    ```bash
    git merge master
    ```
</details>

<details>

<summary>Attaching the debugger manually</summary>

This should not be necessary if you've run the `setupprojects.sh` script.

More instructions can be found here: [check the official docs and use values like this:](https://developer.valvesoftware.com/wiki/Installing_and_Debugging_the_Source_Code)

Setup the debugger with the following values:
> - Command: `C:\Program Files %28x86%29\Steam\steamapps\common\Source SDK Base 2013 Multiplayer\hl2.exe`
> - Command Arguments: `-allowdebug -dev -sw -game "C:\Program Files (x86)\Steam\steamapps\sourcemods\experiment"`
> - Working Directory: `C:\Program Files %28x86%29\Steam\steamapps\common\Source SDK Base 2013 Multiplayer`

</details>
