---
template: lua-class-function.html
title: CanSwitchToWeapon
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Player
  function: CanSwitchToWeapon
  realm: shared
  description: "Check if the player can switch to a weapon."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "weapon"
    type: Weapon
  returns:
    - type: boolean
      description: "Whether the player can switch to the weapon."
---

<div class="lua__search__keywords">
Player:CanSwitchToWeapon &#x2013; Check if the player can switch to a weapon.
</div>
