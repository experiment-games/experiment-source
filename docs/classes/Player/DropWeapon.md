---
template: lua-class-function.html
title: DropWeapon
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Player
  function: DropWeapon
  realm: server
  description: "Drop a weapon for the player."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "weapon"
    type: Weapon
  - position: 3
    name: "position"
    type: Vector
    default: "NULL"
  - position: 4
    name: "velocity"
    type: Vector
    default: "NULL"
  returns:
    
---

<div class="lua__search__keywords">
Player:DropWeapon &#x2013; Drop a weapon for the player.
</div>
