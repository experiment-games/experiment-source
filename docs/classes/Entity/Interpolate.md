---
template: lua-class-function.html
title: Interpolate
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: Interpolate
  realm: client
  description: "Interpolate the entity."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "currentTime"
    type: number
  returns:
    - type: boolean
      description: "Whether the operation was successful"
---

<div class="lua__search__keywords">
Entity:Interpolate &#x2013; Interpolate the entity.
</div>
