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
    - name: "vecRayOrigin"
      type: Vector
    - name: "vecRayDelta"
      type: Vector
    - name: "vecSphereCenter"
      type: Vector
    - name: "flRadius"
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
