---
template: lua-class-function.html
title: GiveItem
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Player
  function: GiveItem
  realm: server
  description: "Give the named weapon to the player."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "name"
    type: string
  - position: 3
    name: "amount"
    type: integer
    default: "0"
  returns:
    - type: Entity
      description: "The item given to the player."
---

<div class="lua__search__keywords">
Player:GiveItem &#x2013; Give the named weapon to the player.
</div>
