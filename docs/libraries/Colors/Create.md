---
template: lua-library-function.html
title: Create
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Colors
  function: Create
  realm: shared
  description: "Creates a new color."
  
  arguments:
  - position: 1
    name: "red"
    type: number
  - position: 2
    name: "green"
    type: number
  - position: 3
    name: "blue"
    type: number
  - position: 4
    name: "alpha"
    type: number
    default: "255"
  returns:
    - type: Color
      description: "The created color."
---

<div class="lua__search__keywords">
Colors.Create &#x2013; Creates a new color.
</div>
