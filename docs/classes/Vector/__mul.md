---
template: lua-class-function.html
title: __mul
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Vector
  function: __mul
  realm: shared
  description: "Multiplies two vectors or a vector by a number."
  
  arguments:
  - position: 1
    types:
      - name: "number"
        type: number
      - name: "vector"
        type: Vector
      - name: "vectorA"
        type: Vector
  - position: 2
    types:
      - name: "vector"
        type: Vector
      - name: "number"
        type: number
      - name: "vectorB"
        type: Vector
  returns:
    - type: Vector
      description: "The product of the two vectors or the vector and the number."
---

<div class="lua__search__keywords">
Vector:__mul &#x2013; Multiplies two vectors or a vector by a number.
</div>
