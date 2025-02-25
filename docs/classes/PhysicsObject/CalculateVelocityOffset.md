---
template: lua-class-function.html
title: CalculateVelocityOffset
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: PhysicsObject
  function: CalculateVelocityOffset
  realm: shared
  description: "Calculates velocity and angular velocity offsets"
  
  arguments:
  - position: 1
    name: "physicsObject"
    type: PhysicsObject
  - position: 2
    name: "velocity"
    type: Vector
  - position: 3
    name: "offset"
    type: Vector
  returns:
    - type: Vector
      description: "The center velocity."
    - type: Vector
      description: "The center angular velocity."
---

<div class="lua__search__keywords">
PhysicsObject:CalculateVelocityOffset &#x2013; Calculates velocity and angular velocity offsets
</div>
