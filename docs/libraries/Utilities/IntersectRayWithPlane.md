---
template: lua-library-function.html
title: IntersectRayWithPlane
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Utilities
  function: IntersectRayWithPlane
  realm: shared
  description: "Intersect a ray with a plane"
  
  arguments:
  - position: 1
    name: "vecRayOrigin"
    type: Vector
  - position: 2
    name: "vecRayDirection"
    type: Vector
  - position: 3
    name: "vecPlaneOrigin"
    type: Vector
  - position: 4
    name: "vecPlaneNormal"
    type: Vector
  returns:
    - type: Vector
      description: "Intersection position"
---

<div class="lua__search__keywords">
Utilities.IntersectRayWithPlane &#x2013; Intersect a ray with a plane
</div>
