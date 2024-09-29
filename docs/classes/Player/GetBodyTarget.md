---
template: lua-class-function.html
title: GetBodyTarget
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Player
  function: GetBodyTarget
  realm: server
  description: "Get the body target of the player."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "target"
    type: Vector
  - position: 3
    name: "isNoisy"
    type: boolean
  returns:
    - type: Vector
      description: "The body target."
---

<div class="lua__search__keywords">
Player:GetBodyTarget &#x2013; Get the body target of the player.
</div>
