---
template: lua-class-function.html
title: BodyTarget
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: BodyTarget
  realm: server
  description: "Body target."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "position"
    type: Vector
  - position: 3
    name: "noisy"
    type: boolean
    default: "true"
  returns:
    - type: Vector
      description: "The body target"
---

<div class="lua__search__keywords">
Entity:BodyTarget &#x2013; Body target.
</div>
