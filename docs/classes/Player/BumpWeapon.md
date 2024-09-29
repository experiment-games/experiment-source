---
template: lua-class-function.html
title: BumpWeapon
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Player
  function: BumpWeapon
  realm: server
  description: "Bump the weapon for the player."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "weapon"
    type: Weapon
  returns:
    - type: boolean
      description: "Whether the weapon was bumped."
---

<div class="lua__search__keywords">
Player:BumpWeapon &#x2013; Bump the weapon for the player.
</div>
