---
template: lua-library-function.html
title: IsObbIntersectingObb
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Utilities
  function: IsObbIntersectingObb
  realm: shared
  description: "Check if an oriented bounding box is intersecting another oriented bounding box"
  
  arguments:
    - name: "vecOrigin1"
      type: Vector
    - name: "vecAngles1"
      type: Angle
    - name: "boxMin1"
      type: Vector
    - name: "boxMax1"
      type: Vector
    - name: "vecOrigin2"
      type: Vector
    - name: "vecAngles2"
      type: Angle
    - name: "boxMin2"
      type: Vector
    - name: "boxMax2"
      type: Vector
    - name: "flTolerance"
      type: number
      default: "0.0f"
  returns:
    - type: boolean
      description: "True if the oriented bounding boxes are intersecting"
---

<div class="lua__search__keywords">
Utilities.IsObbIntersectingObb &#x2013; Check if an oriented bounding box is intersecting another oriented bounding box
</div>
