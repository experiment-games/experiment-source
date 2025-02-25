---
template: lua-class-function.html
title: SetFov
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Player
  function: SetFov
  realm: shared
  description: "Set the player's field of view."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "fov"
    type: number
  - position: 3
    name: "transitionTime"
    type: number
    default: "0"
  - position: 4
    name: "requester"
    type: Entity
    default: "PLAYER_THEMSELVES"
  returns:
    
---

<div class="lua__search__keywords">
Player:SetFov &#x2013; Set the player's field of view.
</div>
