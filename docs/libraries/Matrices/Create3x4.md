---
template: lua-library-function.html
title: Create3x4
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Matrices
  function: Create3x4
  realm: shared
  description: "Creates a new 3x4 matrix."
  
  argumentSets:
    - arguments:
      - name: "vector"
        type: Vector
        nillable: true
      - name: "vector"
        type: Vector
        nillable: true
      - name: "vector"
        type: Vector
        nillable: true
      - name: "vector"
        type: Vector
        nillable: true
    - arguments:
      - name: "number"
        type: number
      - name: "number"
        type: number
      - name: "number"
        type: number
      - name: "number"
        type: number
      - name: "number"
        type: number
      - name: "number"
        type: number
      - name: "number"
        type: number
      - name: "number"
        type: number
      - name: "number"
        type: number
      - name: "number"
        type: number
      - name: "number"
        type: number
      - name: "number"
        type: number
  returns:
    - type: Matrix3x4
      description: "The created 3x4 matrix."
---

<div class="lua__search__keywords">
Matrices.Create3x4 &#x2013; Creates a new 3x4 matrix.
</div>
