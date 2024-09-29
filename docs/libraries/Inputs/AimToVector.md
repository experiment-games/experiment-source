---
template: lua-library-function.html
title: AimToVector
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Inputs
  function: AimToVector
  realm: client
  description: "Convert aiming angles and screen coordinates to a vector."
  
  arguments:
  - position: 1
    name: "angles"
    type: Angle
  - position: 2
    name: "fov"
    type: number
  - position: 3
    name: "x"
    type: number
  - position: 4
    name: "y"
    type: number
  - position: 5
    name: "screenWidth"
    type: number
  - position: 6
    name: "screenHeight"
    type: number
  returns:
    - type: Vector
      description: "Aim vector in world coordinates."
---

<div class="lua__search__keywords">
Inputs.AimToVector &#x2013; Convert aiming angles and screen coordinates to a vector.
</div>
