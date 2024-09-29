---
template: lua-class-function.html
title: Give
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Player
  function: Give
  realm: server
  description: "Give the weapon to the player."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "className"
    type: string
  - position: 3
    name: "shouldGiveNoAmmo"
    type: boolean
    default: "false"
  returns:
    - type: Weapon
      description: "The weapon given to the player."
---

<div class="lua__search__keywords">
Player:Give &#x2013; Give the weapon to the player.
</div>
