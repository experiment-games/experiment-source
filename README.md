# Experiment (Source Engine Mod)

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

2. Open the created `experiment.sln` solution in Visual Studio 2022 (or newer).

    > [!WARNING]
    > Despite building in Visual Studio 2022, the Source SDK 2013 solution requires Visual Studio 2013 to be installed.

3. Build the solution in `Release` mode.

4. After the build is complete make a symlink from the `game/experiment` directory to your `steamapps/sourcemods` directory. E.g:

    ```bash
    mklink /J "C:\Program Files (x86)\Steam\steamapps\sourcemods\experiment" "<path to this repo>\game\experiment"
    ```

5. To setup the game mod directory, run:

    ```bash
    ./tools/setup-game.sh
    ```

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

<summary>Attaching the debugger</summary>

1. Copy the `.env.example` file to `.env`.

    ```bash
    cp .env.example .env
    ```

2. Fill the `.env` file with the described values

3. Then run the `setupprojects.sh` script to modify the Visual Studio project files and solution.

    ```bash
    ./setupprojects.sh
    ```

    _This sets up debugging and enforces the correct dependencies of the client & server projects._

> Setting up debugging manually: [check the official docs and use values like this:](https://developer.valvesoftware.com/wiki/Installing_and_Debugging_the_Source_Code)
>
> - Command: `C:\Program Files %28x86%29\Steam\steamapps\common\Source SDK Base 2013 Multiplayer\hl2.exe`
> - Command Arguments: `-allowdebug -dev -sw -game "C:\Program Files (x86)\Steam\steamapps\sourcemods\experiment"`
> - Working Directory: `C:\Program Files %28x86%29\Steam\steamapps\common\Source SDK Base 2013 Multiplayer`

</details>
