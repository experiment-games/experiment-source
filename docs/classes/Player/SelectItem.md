---
template: lua-class-function.html
title: SelectItem
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Player
  function: SelectItem
  realm: shared
  description: "Select an item for the player."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "item name"
    type: string
  - position: 3
    name: "slot"
    type: number
    default: "0"
  returns:
    
---

<div class="lua__search__keywords">
Player:SelectItem &#x2013; Select an item for the player.
</div>
