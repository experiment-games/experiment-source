---
template: lua-class-function.html
title: RemoveWeapon
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Player
  function: RemoveWeapon
  realm: server
  description: "Remove the weapon for the player."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "weapon"
    type: Weapon
  returns:
    - type: boolean
      description: "Whether the weapon was removed."
---

<div class="lua__search__keywords">
Player:RemoveWeapon &#x2013; Remove the weapon for the player.
</div>
