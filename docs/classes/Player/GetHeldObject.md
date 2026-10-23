---
template: lua-class-function.html
title: GetHeldObject
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Player
  function: GetHeldObject
  realm: server
  description: "Get the held object of the player."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  returns:
    - type: Entity
      description: "The held object, or nil if none."
---

<div class="lua__search__keywords">
Player:GetHeldObject &#x2013; Get the held object of the player.
</div>
