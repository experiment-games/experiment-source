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
  - position: 1
    name: "vecOrigin1"
    type: Vector
  - position: 2
    name: "vecAngles1"
    type: Angle
  - position: 3
    name: "boxMin1"
    type: Vector
  - position: 4
    name: "boxMax1"
    type: Vector
  - position: 5
    name: "vecOrigin2"
    type: Vector
  - position: 6
    name: "vecAngles2"
    type: Angle
  - position: 7
    name: "boxMin2"
    type: Vector
  - position: 8
    name: "boxMax2"
    type: Vector
  - position: 9
    name: "flTolerance"
    type: number
    default: "0.0f"
  returns:
    - type: boolean
      description: "True if the oriented bounding boxes are intersecting"
---

<div class="lua__search__keywords">
Utilities.IsObbIntersectingObb &#x2013; Check if an oriented bounding box is intersecting another oriented bounding box
</div>
