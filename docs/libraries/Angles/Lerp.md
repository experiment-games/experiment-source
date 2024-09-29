---
template: lua-library-function.html
title: Lerp
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Angles
  function: Lerp
  realm: shared
  description: "Linearly interpolates between two angles."
  
  arguments:
  - position: 1
    name: "fraction"
    type: number
  - position: 2
    name: "angleStart"
    type: Angle
  - position: 3
    name: "angleEnd"
    type: Angle
  returns:
    - type: Angle
      description: "The interpolated angle."
---

<div class="lua__search__keywords">
Angles.Lerp &#x2013; Linearly interpolates between two angles.
</div>
