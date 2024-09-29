---
template: lua-library-function.html
title: IsBoxIntersectingSphere
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Utilities
  function: IsBoxIntersectingSphere
  realm: shared
  description: "Check if a box is intersecting a sphere"
  
  arguments:
  - position: 1
    name: "boxMin"
    type: Vector
  - position: 2
    name: "boxMax"
    type: Vector
  - position: 3
    name: "spherePosition"
    type: Vector
  - position: 4
    name: "sphereRadius"
    type: number
  returns:
    - type: boolean
      description: "True if the box is intersecting the sphere"
---

<div class="lua__search__keywords">
Utilities.IsBoxIntersectingSphere &#x2013; Check if a box is intersecting a sphere
</div>
