# Goals & Roadmap

This page outlines the overall goals and roadmap for Experiment. It is a living document and will be updated as the project progresses.

## Goals

In order to provide complete transparency, we have outlined the goals of Experiment below:

1. **Have fun developing this project.**

    This is the primary purpose of Experiment. We want to have fun developing this project.
    We do not have the explicit intention of making money, releasing or maintaining a product, or creating a community.

2. **Provide a platform for prototyping Source Engine mods.**

    We aim to provide a platform for quickly prototyping Source Engine mods without having to write any C++ code.
    This way you can focus on the gameplay and content of your mod, without having to worry about the technical details of the Source Engine.

3. **Have compatibility with Garry's Mod and other tools.**

    We aim to make Experiment as compatible as possible with Garry's Mod Lua scripts.
    This is to ensure that you can use as much of your existing code as possible.

## Roadmap

### **`v0.1`**: Basic Functionality

- [x] **Gamemodes**:

    Gamemodes load and can inherit from each other. Their content is mounted.

- [x] **Addons:**

    Addons are loaded and their content is mounted.

- [x] **Lua binary modules load:**

    Binary Lua modules can be loaded and used in Lua scripts.

- [x] **Garry's Mod Lua binary modules load:**

    Tested with [MySQLOO by FreddyH](https://github.com/FredyH/MySQLOO).

- [ ] **Addon toggling:**

    Addons can be toggled on and off in the main menu.

- [x] **Game content mounting:**

    Content of games installed with Steam can be mounted.
    The content can be toggled on and off in the main menu.

- [x] **VGUI:**

    Basic VGUI elements can be created and displayed on the screen.

- [x] **Garry's Mod Sandbox gamemode functional:**

    The Garry's Mod Sandbox gamemode can be loaded and played.
    Props can be spawned and removed.
    The SpawnMenu UI works.

- [ ] **[Experiment Redux](https://github.com/luttje/gmod-experiment-redux) gamemode works:**

    The original purpose of Experiment was to bring Experiment Redux to its own mod.
    That got out of hand, since we're now building a full-fledged modding platform.

- [x] **Developer Portal:**

    Setup how we can document the Lua API early, rather than waiting until we publicly release Experiment.
    This way we can document the API as we go, reducing the slog of documenting everything at once.
    Additionally this recurring task adds some more variation to our development process.

- [ ] *TODO: More tasks*

### **`v0.2`**: ???

- [ ] *TODO: More tasks*

### **`v0.3` through `v0.8`**: API stabilization

- [ ] **Document and adhere to API conventions:**

    The API should be documented and adhere to a set of conventions.
    This includes naming conventions, parameter order, and return values.
    Developers should consistently know what to expect from the API.

- [ ] **Integration tests:**

    Integration tests written in Lua should be created to ensure that the API works as expected.
    It would be fun to use [Jestronaut](https://github.com/luttje/jestronaut) for this.

- [ ] *TODO: More tasks*

### **`v0.9`**: Low priority, but nice to have

- [ ] **Garry's Mod toolgun works:**

    The Garry's Mod toolgun can be used to manipulate props.

- [ ] *TODO: More tasks*
