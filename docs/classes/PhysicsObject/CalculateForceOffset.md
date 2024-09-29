---
template: lua-class-function.html
title: CalculateForceOffset
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: PhysicsObject
  function: CalculateForceOffset
  realm: shared
  description: "Calculates force and torque offset"
  
  arguments:
  - position: 1
    name: "physicsObject"
    type: PhysicsObject
  - position: 2
    name: "force"
    type: Vector
  - position: 3
    name: "worldPosition"
    type: Vector
  returns:
    - type: Vector
      description: "The center force."
    - type: Vector
      description: "The center torque."
---

<div class="lua__search__keywords">
PhysicsObject:CalculateForceOffset &#x2013; Calculates force and torque offset
</div>
