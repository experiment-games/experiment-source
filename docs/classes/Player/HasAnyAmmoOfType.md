---
template: lua-class-function.html
title: HasAnyAmmoOfType
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Player
  function: HasAnyAmmoOfType
  realm: server
  description: "Check if the player has any ammo of the specified type."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "type"
    type: number
  returns:
    - type: boolean
      description: "Whether the player has any ammo of the specified type."
---

<div class="lua__search__keywords">
Player:HasAnyAmmoOfType &#x2013; Check if the player has any ammo of the specified type.
</div>
