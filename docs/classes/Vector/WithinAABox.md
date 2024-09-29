---
template: lua-class-function.html
title: WithinAABox
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Vector
  function: WithinAABox
  realm: shared
  description: "Checks if the vector is within an axis-aligned box."
  
  arguments:
  - position: 1
    name: "vector"
    type: Vector
  - position: 2
    name: "boxMin"
    type: Vector
  - position: 3
    name: "boxMax"
    type: Vector
  returns:
    - type: boolean
      description: "True if the vector is within the axis-aligned box, false otherwise."
---

<div class="lua__search__keywords">
Vector:WithinAABox &#x2013; Checks if the vector is within an axis-aligned box.
</div>
