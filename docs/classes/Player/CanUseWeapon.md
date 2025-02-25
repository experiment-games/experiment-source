---
template: lua-class-function.html
title: CanUseWeapon
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Player
  function: CanUseWeapon
  realm: server
  description: "Check if the player can use a weapon."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "weapon"
    type: Weapon
  returns:
    - type: boolean
      description: "Whether the player can use the weapon."
---

<div class="lua__search__keywords">
Player:CanUseWeapon &#x2013; Check if the player can use a weapon.
</div>
