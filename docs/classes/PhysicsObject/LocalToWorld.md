---
template: lua-class-function.html
title: LocalToWorld
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: PhysicsObject
  function: LocalToWorld
  realm: shared
  description: "Converts local position to world position"
  
  arguments:
  - position: 1
    name: "physicsObject"
    type: PhysicsObject
  - position: 2
    name: "localPosition"
    type: Vector
  returns:
    - type: Vector
      description: "The world position."
---

<div class="lua__search__keywords">
PhysicsObject:LocalToWorld &#x2013; Converts local position to world position
</div>
