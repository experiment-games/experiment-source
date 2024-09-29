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
  library: Vectors
  function: Create
  realm: shared
  description: "Creates a new vector."
  
  arguments:
  - position: 1
    types:
      - name: "vector"
        type: Vector
      - name: "vectorString"
        type: string
      - name: "x"
        type: number
    default: "0.0f"
  - position: 2
    name: "y"
    type: number
    default: "0.0f"
  - position: 3
    name: "z"
    type: number
    default: "0.0f"
  returns:
    - type: Vector
      description: "The created vector."
---

<div class="lua__search__keywords">
Vectors.Create &#x2013; Creates a new vector.
</div>
