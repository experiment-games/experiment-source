---
template: lua-class-function.html
title: GetColor
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Scheme
  function: GetColor
  realm: client
  description: "Returns the color for the specified color name."
  
  arguments:
  - position: 1
    name: "scheme"
    type: Scheme
  - position: 2
    name: "colorName"
    type: string
  - position: 3
    name: "defaultColor"
    type: Color
  returns:
    - type: color
      description: "The color for the specified color name."
---

<div class="lua__search__keywords">
Scheme:GetColor &#x2013; Returns the color for the specified color name.
</div>
