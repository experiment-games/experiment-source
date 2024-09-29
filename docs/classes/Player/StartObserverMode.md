---
template: lua-class-function.html
title: StartObserverMode
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Player
  function: StartObserverMode
  realm: server
  description: "Start the observer mode for the player."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "mode"
    type: enumeration/OBSERVER_MODE
  returns:
    - type: boolean
      description: "Whether the observer mode was started."
---

<div class="lua__search__keywords">
Player:StartObserverMode &#x2013; Start the observer mode for the player.
</div>
