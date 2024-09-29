---
template: lua-class-function.html
title: HasWeapon
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Player
  function: HasWeapon
  realm: shared
  description: "Check if the player has the named weapon."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "name"
    type: string
  returns:
    - type: boolean
      description: "Whether the player has the weapon."
---

<div class="lua__search__keywords">
Player:HasWeapon &#x2013; Check if the player has the named weapon.
</div>
