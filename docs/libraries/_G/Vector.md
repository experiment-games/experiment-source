---
template: lua-library-function.html
title: Vector
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: _G
  function: Vector
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
    - type: vector
      description: "The created vector."
---

<div class="lua__search__keywords">
_G.Vector &#x2013; Creates a new vector.
</div>
