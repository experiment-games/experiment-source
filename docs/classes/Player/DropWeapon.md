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
    - name: "weapon"
      type: Weapon
    - name: "position"
      type: Vector
      default: "NULL"
    - name: "velocity"
      type: Vector
      default: "NULL"
  returns:
    
---

<div class="lua__search__keywords">
Player:DropWeapon &#x2013; Drop a weapon for the player.
</div>
