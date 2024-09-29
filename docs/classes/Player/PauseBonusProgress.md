---
template: lua-class-function.html
title: PauseBonusProgress
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Player
  function: PauseBonusProgress
  realm: server
  description: "Pause or unpause the bonus progress for the player."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "shouldPause"
    type: boolean
    default: "true"
  returns:
    
---

<div class="lua__search__keywords">
Player:PauseBonusProgress &#x2013; Pause or unpause the bonus progress for the player.
</div>
