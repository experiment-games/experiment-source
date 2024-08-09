# Garry's Mod Compatibility

!!! Note We are not affiliated with Garry's Mod or [Facepunch
    Studios](https://facepunch.com/).

    No code from Garry's Mod is provided with Experiment. You can however choose
    to mount Garry's Mod content in Experiment.
    This includes Lua scripts, addons, and other content from Garry's Mod.

## Current Compatibility

Compatibility with Garry's Mod is an ongoing effort. Currently compatibility
with Garry's Mod is implemented in the following ways:

- The [gamemode](../general/concepts/gamemodes.md) system works similarly
- The [addon](../general/concepts/addons.md) system works similarly
- The [hook](../general/concepts/hooks.md) system works similarly
- In some ways the Lua API is similar to Garry's Mod's Lua API out of the box
- This module renames and reimplements some Garry's Mod functions
- Where Garry's Mod has a `lua` folder Experiment places lua scripts in
  `scripts/lua`.
- Instead of `lua/autorun` we use `scripts/lua/autorun` for scripts that should
  run automatically.
- This module loads scripts in the Garry's Mod addons `lua/` folder after it has
  setup compatibility with Garry's Mod.
- We reimplemented the Lua 5.1 `module` functionality despite us using Lua 5.4.
- Similar to Garry's Mod we implemented the C-style operators `!` for 'logical
  not', `&&` for 'logical and', `||` for 'logical or', `//` and `/* */` for
  comments.
