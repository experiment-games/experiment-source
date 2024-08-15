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
    - name: "angles"
      type: Angle
    - name: "fov"
      type: number
    - name: "x"
      type: number
    - name: "y"
      type: number
    - name: "screenWidth"
      type: number
    - name: "screenHeight"
      type: number
  returns:
    - type: Vector
      description: "Aim vector in world coordinates."
---

<div class="lua__search__keywords">
Inputs.AimToVector &#x2013; Convert aiming angles and screen coordinates to a vector.
</div>
