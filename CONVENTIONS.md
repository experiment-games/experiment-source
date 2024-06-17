# 📔 Conventions

> [!WARNING]
> This document is a work in progress and it is not complete.

This document contains the conventions we use for [Lua](#lua) and [C++](#c) code.

## Common

### General

- Use English for all code, comments, and documentation.

- Write descriptive comments for code that is not self-explanatory.

- Prefix comments with `TODO` for tasks that need to be done.

- Prefix comments with `Experiment;` to indicate we added the comment. This helps others reach us if they have questions.

## Lua

### Code style

See the `.editorconfig` file for the code style.

### Naming conventions

- Files are always written in `snake_case`:

  - `player.lua`
  - `weapon.lua`
  - `player_health.lua`

- Files that should not be included (except by a specific script) are prefixed with an underscore:

  - `_config.lua`
  - `_settings.lua`

- Gamemode files should prefix the realm:

  - `sh_` for shared files
  - `cl_` for client files
  - `sv_` for server files

- Variables and functions are not abbreviated, with the exception of these abbreviations:

  - `Hud`
  - `Api`
  - `Id`
  - `Url`
  - `Json`, `Xml`, `Html`, `Css` and other data format/language names

- Variables and functions are preferred to be descriptive over short.

- Local variables, including function arguments, are written in `lowerCamelCase`:

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

- When we extend a base Lua library, we also write the function in `UpperCamelCase`.

- We use an underscore to prevent conflicts between variables and base libraries:

  ```lua
  function table.Print(_table)
    print("Table:")
    PrintTable(_table)
  end
  ```

- Loop variables are written in `lowerCamelCase` and should be descriptive.
  We use the `_` variable for unused variables:

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

- Metamethod methods always get the object as the first argument, so use the `:` operator:

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

- Constants, or variables that should not be modified, are written in `UPPER_SNAKE_CASE`:

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

TODO
