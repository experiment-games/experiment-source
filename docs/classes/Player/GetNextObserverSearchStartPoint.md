---
template: lua-class-function.html
title: GetNextObserverSearchStartPoint
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Player
  function: GetNextObserverSearchStartPoint
  realm: server
  description: "Get the next observer search start point for the player."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "reverse"
    type: boolean
  returns:
    - type: integer
      description: "The next observer search start point."
---

<div class="lua__search__keywords">
Player:GetNextObserverSearchStartPoint &#x2013; Get the next observer search start point for the player.
</div>
