---
template: lua-class-function.html
title: GetEyePositionAndVectors
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Player
  function: GetEyePositionAndVectors
  realm: shared
  description: "Get the player's eye position and vectors."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  returns:
    - type: Vector
      description: "The player's eye position."
    - type: Vector
      description: "The player's eye forward vector."
    - type: Vector
      description: "The player's eye right vector."
    - type: Vector
      description: "The player's eye up vector."
---

<div class="lua__search__keywords">
Player:GetEyePositionAndVectors &#x2013; Get the player's eye position and vectors.
</div>
