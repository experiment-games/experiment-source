---
template: lua-class-function.html
title: ShouldSetLastWeapon
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Player
  function: ShouldSetLastWeapon
  realm: shared
  description: "Check if the player should set the last weapon."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "weapon"
    type: Weapon
  - position: 3
    name: "last"
    type: Weapon
  returns:
    - type: boolean
      description: "Whether the player should set the last weapon."
---

<div class="lua__search__keywords">
Player:ShouldSetLastWeapon &#x2013; Check if the player should set the last weapon.
</div>
