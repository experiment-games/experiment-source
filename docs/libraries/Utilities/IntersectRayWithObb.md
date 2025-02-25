---
template: lua-library-function.html
title: IntersectRayWithObb
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Utilities
  function: IntersectRayWithObb
  realm: shared
  description: "Intersect a ray with an oriented bounding box"
  
  arguments:
  - position: 1
    name: "vecRayStart"
    type: Vector
  - position: 2
    name: "vecRayDelta"
    type: Vector
  - position: 3
    name: "vecBoxOrigin"
    type: Vector
  - position: 4
    name: "angBoxAngles"
    type: Angle
  - position: 5
    name: "vecBoxMins"
    type: Vector
  - position: 6
    name: "vecBoxMaxs"
    type: Vector
  returns:
    - type: Vector
      description: "End position of the intersection"
    - type: Vector
      description: "Normal of the intersection"
    - type: number
      description: "Fraction of the intersection"
---

<div class="lua__search__keywords">
Utilities.IntersectRayWithObb &#x2013; Intersect a ray with an oriented bounding box
</div>
