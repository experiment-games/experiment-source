---
template: lua-class-function.html
title: Kill
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Player
  function: Kill
  realm: server
  description: "Force the player character to die, optionally with an explosion."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "shouldExplode"
    type: boolean
    default: "false"
  returns:
    
---

<div class="lua__search__keywords">
Player:Kill &#x2013; Force the player character to die, optionally with an explosion.
</div>
