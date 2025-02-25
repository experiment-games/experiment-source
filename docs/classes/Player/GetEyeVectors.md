---
template: lua-class-function.html
title: GetEyeVectors
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Player
  function: GetEyeVectors
  realm: shared
  description: "Get the player's eye vectors."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  returns:
    - type: Vector
      description: "The player's eye forward vector."
    - type: Vector
      description: "The player's eye right vector."
    - type: Vector
      description: "The player's eye up vector."
---

<div class="lua__search__keywords">
Player:GetEyeVectors &#x2013; Get the player's eye vectors.
</div>
