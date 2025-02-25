---
template: lua-class-function.html
title: IsIlluminatedByFlashlight
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Player
  function: IsIlluminatedByFlashlight
  realm: server
  description: "Check if the player is illuminated by the flashlight."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "entity"
    type: Entity
  returns:
    - type: boolean
      description: "Whether the player is illuminated by the flashlight."
    - type: number
      description: "The dot product."
---

<div class="lua__search__keywords">
Player:IsIlluminatedByFlashlight &#x2013; Check if the player is illuminated by the flashlight.
</div>
