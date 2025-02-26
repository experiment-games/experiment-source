# Scripts

In Experiment Source, Lua scripts define functionality and gameplay logic. These scripts are stored in the `<experiment directory>/scripts/lua/auto_load` directory. Scripts in this directory are automatically loaded when the game starts on [both the client and server realms](realms.md).

To create auto-loaded scripts for only the client or server, place them in the `<experiment directory>/scripts/lua/client` or `<experiment directory>/scripts/lua/server` directories, respectively.

## Writing Scripts

Scripts are written in Lua, a lightweight, high-level programming language. Lua is easy to learn and use, making it an excellent choice for modding and game development.

### Example

Here is a simple script that prints a message to the console when the game starts:

```lua
print("Hello, world!")
```

Place this script in the `<experiment directory>/scripts/lua/auto_load` directory. Then start Experiment Source to see the message in the console.

### Code Editor Recommendations

When writing Lua scripts, we recommend using a code editor with syntax highlighting and code completion. Some popular code editors for Lua development include:

- (Recommended) [Visual Studio Code](https://code.visualstudio.com/)

- [NeoVim](https://neovim.io/)

- [Sublime Text](https://www.sublimetext.com/)

We recommend Visual Studio Code for its extensive Lua support and ease of use. You can install the [Lua Language Server](https://marketplace.visualstudio.com/items?itemName=sumneko.lua) extension to enhance your Lua development experience.

??? note "Future Support"
    In the future, we plan to provide definitions for our API in the form of a [LuaLS Addon](https://luals.github.io/wiki/addons/), similar to [this LuaLS Addon for Garry's Mod](https://github.com/luttje/glua-api-snippets)
