---
template: lua-class-function.html
title: ShouldSelectWeapon
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Player
  function: ShouldSelectWeapon
  realm: shared
  description: "Check if the player should select a weapon."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "weapon"
    type: Weapon
  returns:
    - type: boolean
      description: "Whether the player should select the weapon."
---

<div class="lua__search__keywords">
Player:ShouldSelectWeapon &#x2013; Check if the player should select a weapon.
</div>
