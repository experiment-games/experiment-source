---
template: lua-class-function.html
title: OwnsWeaponOfType
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Player
  function: OwnsWeaponOfType
  realm: shared
  description: "Check if the player owns a weapon of a certain type."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "type"
    type: string
  - position: 3
    name: "subType"
    type: number
    default: "0"
  returns:
    - type: Weapon
      description: "The weapon the player owns of the specified type."
---

<div class="lua__search__keywords">
Player:OwnsWeaponOfType &#x2013; Check if the player owns a weapon of a certain type.
</div>
