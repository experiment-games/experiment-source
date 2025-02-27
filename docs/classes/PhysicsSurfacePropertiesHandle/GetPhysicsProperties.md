---
template: lua-class-function.html
title: GetPhysicsProperties
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: PhysicsSurfacePropertiesHandle
  function: GetPhysicsProperties
  realm: shared
  description: "Gets physics properties"
  
  arguments:
  - position: 1
    name: "physprops"
    type: PhysicsSurfacePropertiesHandle
  - position: 2
    name: "surfaceDataIndex"
    type: number
  returns:
    - type: number
      description: "The density."
    - type: number
      description: "The thickness."
    - type: number
      description: "The friction."
    - type: number
      description: "The elasticity."
---

<div class="lua__search__keywords">
PhysicsSurfacePropertiesHandle:GetPhysicsProperties &#x2013; Gets physics properties
</div>
