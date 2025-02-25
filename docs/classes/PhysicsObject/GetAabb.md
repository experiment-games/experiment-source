---
template: lua-class-function.html
title: GetAabb
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: PhysicsObject
  function: GetAabb
  realm: shared
  description: "Gets the axis-aligned bounding box"
  
  arguments:
  - position: 1
    name: "collide"
    type: PhysicsObject
  returns:
    - type: Vector
      description: "The minimum vector."
    - type: Vector
      description: "The maximum vector."
---

<div class="lua__search__keywords">
PhysicsObject:GetAabb &#x2013; Gets the axis-aligned bounding box
</div>
