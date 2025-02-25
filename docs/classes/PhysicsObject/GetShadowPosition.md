---
template: lua-class-function.html
title: GetShadowPosition
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: PhysicsObject
  function: GetShadowPosition
  realm: shared
  description: "Gets the shadow position and angles"
  
  arguments:
  - position: 1
    name: "physicsObject"
    type: PhysicsObject
  returns:
    - type: boolean
      description: "Whether there is a shadow position or not."
    - type: Vector
      description: "The shadow position."
    - type: QAngle
      description: "The angles."
---

<div class="lua__search__keywords">
PhysicsObject:GetShadowPosition &#x2013; Gets the shadow position and angles
</div>
