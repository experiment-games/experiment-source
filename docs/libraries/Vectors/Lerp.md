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
  library: Vectors
  function: Lerp
  realm: shared
  description: "Linearly interpolates between two vectors."
  
  arguments:
  - position: 1
    name: "fraction"
    type: number
  - position: 2
    name: "from"
    type: Vector
  - position: 3
    name: "to"
    type: Vector
  returns:
    - type: Vector
      description: "The interpolated vector."
---

<div class="lua__search__keywords">
Vectors.Lerp &#x2013; Linearly interpolates between two vectors.
</div>
