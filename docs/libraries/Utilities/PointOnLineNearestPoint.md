---
template: lua-library-function.html
title: PointOnLineNearestPoint
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Utilities
  function: PointOnLineNearestPoint
  realm: shared
  description: "Returns the nearest point on a line to a given point."
  
  arguments:
  - position: 1
    name: "point"
    type: Vector
  - position: 2
    name: "lineStart"
    type: Vector
  - position: 3
    name: "lineEnd"
    type: Vector
  - position: 4
    name: "clamp"
    type: boolean
    default: "false"
  returns:
    - type: Vector
      description: "The nearest point on the line."
---

<div class="lua__search__keywords">
Utilities.PointOnLineNearestPoint &#x2013; Returns the nearest point on a line to a given point.
</div>
