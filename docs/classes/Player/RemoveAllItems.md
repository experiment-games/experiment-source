---
template: lua-class-function.html
title: RemoveAllItems
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Player
  function: RemoveAllItems
  realm: server
  description: "Remove all items from the player."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "shouldRemoveSuit"
    type: boolean
    default: "false"
  returns:
    
---

<div class="lua__search__keywords">
Player:RemoveAllItems &#x2013; Remove all items from the player.
</div>
