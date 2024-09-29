---
template: lua-class-function.html
title: IsEqualWithTolerance
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Angle
  function: IsEqualWithTolerance
  realm: shared
  description: "Checks if the angle is equal to another angle within a tolerance."
  
  arguments:
  - position: 1
    name: "angle"
    type: Angle
  - position: 2
    name: "other"
    type: Angle
  - position: 3
    name: "tolerance"
    type: number
  returns:
    - type: boolean
      description: "True if the angles are equal within the tolerance, false otherwise."
---

<div class="lua__search__keywords">
Angle:IsEqualWithTolerance &#x2013; Checks if the angle is equal to another angle within a tolerance.
</div>
