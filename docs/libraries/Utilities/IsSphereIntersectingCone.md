---
template: lua-library-function.html
title: IsSphereIntersectingCone
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Utilities
  function: IsSphereIntersectingCone
  realm: shared
  description: "Check if a sphere is intersecting a cone"
  
  arguments:
  - position: 1
    name: "sphereCenter"
    type: Vector
  - position: 2
    name: "sphereRadius"
    type: number
  - position: 3
    name: "coneOrigin"
    type: Vector
  - position: 4
    name: "coneAxis"
    type: Vector
  - position: 5
    name: "coneSine"
    type: number
  - position: 6
    name: "coneCosine"
    type: number
  returns:
    - type: boolean
      description: "True if the sphere is intersecting the cone"
---

<div class="lua__search__keywords">
Utilities.IsSphereIntersectingCone &#x2013; Check if a sphere is intersecting a cone
</div>
