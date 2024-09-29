---
template: lua-class-function.html
title: __eq
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Angle
  function: __eq
  realm: shared
  description: "Metatable that is called when the angle is to be compared with another angle."
  
  arguments:
  - position: 1
    name: "angle"
    type: Angle
  - position: 2
    name: "other"
    type: Angle
  returns:
    - type: boolean
      description: "True if the angles are equal, false otherwise."
---

<div class="lua__search__keywords">
Angle:__eq &#x2013; Metatable that is called when the angle is to be compared with another angle.
</div>
