# 📔 Conventions

!!! warning

    This document is a work in progress and it is not complete.

This document contains the conventions we use for [Lua](#lua) and [C++](#c)
code.

## Common

### General

- Use American English for all code, comments, and documentation.

    For example:

    - `color` instead of `colour`
    - `center` instead of `centre`
    - `armor` instead of `armour`
    - `behavior` instead of `behaviour`

- Write descriptive comments for code that is not self-explanatory.

- Prefix comments with `Experiment;` to indicate we added the comment. This
  helps others reach us if they have questions.

- Prefix comments with `Experiment; TODO` for tasks that need to be done.

## Lua

### Code style

See the `.editorconfig` file for the code style.

Additionally, we use the following conventions:

- Use `--` for single line comments.

- Use `--[[` and `]]` for multi-line comments.

- Use `local` for all variables, unless they are intended to be global.

- Use double quotes for strings, e.g. `"Hello, world!"`.

### Naming conventions

- Lua files are always written in `snake_case`:

    - `player.lua`
    - `weapon.lua`
    - `player_health.lua`

- Files that should not be included (except by a specific script) are prefixed
  with an underscore:

    - `_config.lua`
    - `_settings.lua`

- Lua files should prefix the realm they are loaded in:

    - `sh_` for shared files
    - `cl_` for client files
    - `sv_` for server files

- Variables and functions are not abbreviated, with the exception of these
  common abbreviations:

    - `2d`, `3d`, `4d` and other dimensions
    - `Aabb` (short for axis-aligned bounding box)
    - `Api`
    - `App` (short for application)
    - `Config`
    - `Demo`
    - `Dev`
    - `Dx` (DirectX), `HDR` and other graphic settings
    - `Fcvar` (short for flag convar, kept this way due to the source engine
        using this abbreviation when explaining convars)
    - `Hltv` (short for Half-Life TV)
    - `Hud`
    - `Hwnd` (short for window handle)
    - `Id` (short for identifier)
    - `Ik` (short for inverse kinematics)
    - `Ime` (short for input method editor)
    - `Json`, `Xml`, `Html`, `Css` and other data format/language names
    - `Obb` (short for oriented bounding box)
    - `Rgb`, `Rgba`, `Hsv`, `Hsl` and other color spaces
    - `Sqr` (short for square root)
    - `Ui`
    - `Url`
    - `Vcd` (short for Valve Choreography Data) and other valve formats (e.g.
      `Vmt`, `Vtf`, `Vpk`, `Mdl`, etc.)
    - `Vgui`

- Abbreviations should be treated as words when capitalizing:

    - `jsonToTable`
    - `tableToJson`
    - `htmlToText`
    - `textToHtml`
    - `JSON_FOR_HUD`

- Note that the following combinations of words are a single word:

    - `Noclip` (not `NoClip`)
    - `Hitbox`
    - `Skybox`
    - `Physcannon` (and other made up names for weapons)

- Variables and functions are preferred to be descriptive over being short.

- Local variables, including function arguments, are written in
  `lowerCamelCase`:

    ```lua
    local playerHealth = 100

    local function getPlayerHealth(healthFraction)
      return playerHealth * healthFraction
    end
    ```

- Variables are not prefixed with `m_` or nor with a type prefix.

- Global variables and functions are written in `UpperCamelCase`:

    ```lua
    LastPlayerDiedAt = os.time()

    function GetPlayerHealth(healthFraction)
      return playerHealth * healthFraction
    end
    ```

- Functions are preferred to be stored in a library.

- Library names are always plural, one word and written in `UpperCamelCase`:

    *(To avoid conflicts with variables like `player` and `entity`)*

- Library functions are written in `UpperCamelCase`:

    ```lua
    local Players = {}
    local Weapons = {}

    function Players.CalculateHealth(healthFraction)
      return playerHealth * healthFraction
    end

    function Weapons.CalculateDamage(damageFraction)
      return weaponDamage * damageFraction
    end
    ```

- When we extend a base Lua library, we also write the function in
  `UpperCamelCase`.

- We use an underscore to prevent conflicts between variables and base
  libraries:

    ```lua
    function table.Print(_table)
      print("Table:")
      PrintTable(_table)
    end
    ```

- Loop variables are written in `lowerCamelCase` and should be descriptive. We
  use the `_` variable for unused variables:

    ```lua
    for _, player in ipairs(players) do
      print(player:Name())
    end

    -- Incorrect:
    for k, v in ipairs(players) do -- wrong
      print(v:Name())
    end
    ```

- Metamethod definitions are written in `UPPER_SNAKE_CASE`.

- Metamethod methods always get the object as the first argument, so use the `:`
  operator:

    ```lua
    local PLAYER = _R.CBasePlayer

    function PLAYER:OnSpawn()
      print("Player " .. self:Name() .. " spawned")
    end

    -- Incorrect:
    function PLAYER.OnSpawn(pl) -- wrong
      print("Player " .. pl:Name() .. " spawned")
    end
    ```

- Constants, or variables that should not be modified, are written in
  `UPPER_SNAKE_CASE`:

    ```lua
    MAX_HEALTH = 100

    -- And constants like:
    -- GAMEMODE
    -- GM
    -- SWEP
    -- ENT
    ```

- Functions that return a boolean value should be named as a question:

    ```lua
    function isPlayerAlive()
      return playerHealth > 0
    end

    function hasPlayerDied()
      return playerHealth <= 0
    end

    function canPlayerRespawn()
      return playerHealth <= 0
    end
    ```

- Enumerations must be written in `UPPER_SNAKE_CASE`:

    ```lua
    local WEAPON_TYPE = {
      MELEE = 1,
      RANGED = 2,
      MAGIC = 3
    }
    ```

- Enumerations must be grouped under a single table:

    ```lua
    local WEAPON_TYPE = {
      MELEE = 1,
      RANGED = 2,
      MAGIC = 3
    }

    local WEAPON = {
      Type = 1,
      Damage = 2,
      Range = 4
    }
    ```

### Common variable names

- A subject player: `client`

- A subject entity: `entity`

- A subject weapon: `weapon`

- When there's multiple of the same type, prefer descriptive names:

    ```lua
    -- When talking about who can hear who:
    function canHearPlayer(clientListening, clientSpeaking)
      return clientListening:CanHear(clientSpeaking)
    end
    ```

- When modifying a number with a fraction, prefer `fraction`:

    ```lua
    function getPlayerHealth(healthFraction)
      return playerHealth * healthFraction
    end

    -- Instead of:
    function getPlayerHealth(healthModifier)
      return playerHealth * healthModifier
    end
    ```

## C++

### Code style

See the `.clang-format` file for the code style.

### Naming conventions

Currently there are a lot of conflicting naming conventions in the C++ codebase.
There's hungarian notation, camelCase, snake_case, and UpperCamelCase. We should
standardize this.

For now let's set this as the standard going forward:

- Classes are written in `UpperCamelCase`:

    ```cpp
    class Player {
    public:
      void Spawn();
    };
    ```

- Functions are written in `UpperCamelCase`:

    ```cpp
    void Player::Spawn() {
      // ...
    }
    ```

- Variables are written in `lowerCamelCase`:

    ```cpp
    int playerHealth = 100;
    ```

- Constants are written in `UPPER_SNAKE_CASE`:

    ```cpp
    const int MAX_HEALTH = 100;
    ```

- Enumerations are written in `UPPER_SNAKE_CASE`.

- Enumerations are always namespaced:

    ```cpp
    namespace WeaponType {
      enum Type {
        MELEE,
        RANGED,
        MAGIC
      };
    }
    ```

    *(This is to prevent conflicts with other enumerations)*

- Functions that return a boolean value should be named as a question:

    ```cpp
    bool IsPlayerAlive() {
      return playerHealth > 0;
    }

    bool HasPlayerDied() {
      return playerHealth <= 0;
    }

    bool WasKeyPressed(keyCode) {
      return keyState[keyCode] == KEY_PRESSED;
    }

    bool CanPlayerRespawn() {
      return playerHealth <= 0;
    }
    ```

- See the Lua naming conventions above for more naming conventions, like:

      - Abbreviations (don't abbreviate, with exceptions)
      - Descriptive names (prefer descriptive names over short names)

- In contrast to the Lua convention, we use the `m_` prefix for member variables:

    ```cpp
    class Player {
    private:
      int m_health = 100;
    };
    ```

- In contrast to the Lua convention, we use the `g_` prefix for global variables:

    ```cpp
    int g_lastPlayerDiedAt = time(nullptr);
    ```

    *(This doesn't apply to global functions)*

!!! todo

    Add more naming conventions for C++.
