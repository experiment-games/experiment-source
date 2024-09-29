---
template: lua-class-function.html
title: ClampCycle
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: ClampCycle
  realm: client
  description: "Clamp the cycle."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "cycle"
    type: number
  - position: 3
    name: "isLooping"
    type: boolean
  returns:
    - type: number
      description: "The clamped cycle"
---

<div class="lua__search__keywords">
Entity:ClampCycle &#x2013; Clamp the cycle.
</div>
