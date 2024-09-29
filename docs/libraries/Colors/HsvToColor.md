---
template: lua-library-function.html
title: HsvToColor
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Colors
  function: HsvToColor
  realm: shared
  description: "Converts HSV to RGB."
  
  arguments:
  - position: 1
    name: "hue"
    type: number
  - position: 2
    name: "saturation"
    type: number
  - position: 3
    name: "value"
    type: number
  returns:
    - type: Color
      description: "The converted color."
---

<div class="lua__search__keywords">
Colors.HsvToColor &#x2013; Converts HSV to RGB.
</div>
