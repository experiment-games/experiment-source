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
    - name: "sphereCenter"
      type: Vector
    - name: "sphereRadius"
      type: number
    - name: "coneOrigin"
      type: Vector
    - name: "coneAxis"
      type: Vector
    - name: "coneSine"
      type: number
    - name: "coneCosine"
      type: number
  returns:
    - type: boolean
      description: "True if the sphere is intersecting the cone"
---

<div class="lua__search__keywords">
Utilities.IsSphereIntersectingCone &#x2013; Check if a sphere is intersecting a cone
</div>
