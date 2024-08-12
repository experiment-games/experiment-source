---
template: lua-library-function.html
title: Color
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: _G
  function: Color
  realm: shared
  description: "Creates a new color."
  
  arguments:
    - name: "red"
      type: number
    - name: "green"
      type: number
    - name: "blue"
      type: number
    - name: "alpha"
      type: number
      default: "255"
  returns:
    - type: color
      description: "The created color."
---

<div class="lua__search__keywords">
_G.Color &#x2013; Creates a new color.
</div>
