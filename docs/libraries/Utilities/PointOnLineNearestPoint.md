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
    - name: "point"
      type: Vector
    - name: "lineStart"
      type: Vector
    - name: "lineEnd"
      type: Vector
    - name: "clamp"
      type: boolean
      default: "false"
  returns:
    - type: vector
      description: "The nearest point on the line."
---

<div class="lua__search__keywords">
Utilities.PointOnLineNearestPoint &#x2013; Returns the nearest point on a line to a given point.
</div>
