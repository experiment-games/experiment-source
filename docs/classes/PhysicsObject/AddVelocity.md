---
template: lua-class-function.html
title: AddVelocity
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: PhysicsObject
  function: AddVelocity
  realm: shared
  description: "Adds velocity and optionally angular velocity"
  
  arguments:
  - position: 1
    name: "physicsObject"
    type: PhysicsObject
  - position: 2
    name: "velocity"
    type: Vector
  - position: 3
    name: "angularVelocity"
    type: Vector
    default: "&angularVelocity"
  returns:
    
---

<div class="lua__search__keywords">
PhysicsObject:AddVelocity &#x2013; Adds velocity and optionally angular velocity
</div>
