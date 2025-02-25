---
template: lua-class-function.html
title: GetAimVector
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Player
  function: GetAimVector
  realm: shared
  description: "Get the player's autoaim vector."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "scale"
    type: number
    default: "1"
  returns:
    - type: Vector
      description: "The player's autoaim vector."
---

<div class="lua__search__keywords">
Player:GetAimVector &#x2013; Get the player's autoaim vector.
</div>
