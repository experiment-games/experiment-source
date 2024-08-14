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
    - name: "vecRayStart"
      type: Vector
    - name: "vecRayDelta"
      type: Vector
    - name: "vecBoxOrigin"
      type: Vector
    - name: "angBoxAngles"
      type: Angle
    - name: "vecBoxMins"
      type: Vector
    - name: "vecBoxMaxs"
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
