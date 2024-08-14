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
    - name: "entity"
      type: Entity
    - name: "fov"
      type: number
    - name: "zoom rate"
      type: number
    - name: "zoom time"
      type: number
      default: "0"
  returns:
    
---

<div class="lua__search__keywords">
Player:SetFov &#x2013; Set the player's field of view.
</div>
