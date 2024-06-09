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

- Variables and functions are not abbreviated.

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

- Library names are always plural and one word.

- Library functions are written in `UpperCamelCase`:

  ```lua
  local players = {}
  local weapons = {}

  function players.GetHealth(healthFraction)
    return playerHealth * healthFraction
  end

  function weapons.GetDamage(damageFraction)
    return weaponDamage * damageFraction
  end
  ```

  *(To avoid conflicts with variables like `player` and `entity`)*

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
