---
template: lua-library-function.html
title: ScreenToWorld
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Inputs
  function: ScreenToWorld
  realm: client
  description: "Convert screen coordinates to world coordinates."
  
  arguments:
  - position: 1
    name: "x"
    type: number
  - position: 2
    name: "y"
    type: number
  returns:
    - type: Vector
      description: "World coordinates vector."
---

<div class="lua__search__keywords">
Inputs.ScreenToWorld &#x2013; Convert screen coordinates to world coordinates.
</div>
