---
template: lua-library-function.html
title: IntersectRayWithSphere
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Utilities
  function: IntersectRayWithSphere
  realm: shared
  description: "Intersect a ray with a sphere"
  
  arguments:
  - position: 1
    name: "vecRayOrigin"
    type: Vector
  - position: 2
    name: "vecRayDelta"
    type: Vector
  - position: 3
    name: "vecSphereCenter"
    type: Vector
  - position: 4
    name: "flRadius"
    type: number
  returns:
    - type: number
      description: "First intersection point"
    - type: number
      description: "Second intersection point"
---

<div class="lua__search__keywords">
Utilities.IntersectRayWithSphere &#x2013; Intersect a ray with a sphere
</div>
