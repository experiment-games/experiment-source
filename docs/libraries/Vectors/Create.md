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
  
  argumentSets:
    - arguments:
      - name: "vector"
        type: Vector
    - arguments:
      - name: "vectorString"
        type: string
    - arguments:
      - name: "x"
        type: number
        default: "0.0f"
      - name: "y"
        type: number
        default: "0.0f"
      - name: "z"
        type: number
        default: "0.0f"
  returns:
    - type: Vector
      description: "The created vector."
---

<div class="lua__search__keywords">
Vectors.Create &#x2013; Creates a new vector.
</div>
