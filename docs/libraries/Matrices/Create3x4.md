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
      - name: "xAxis"
        type: Vector
        nillable: true
      - name: "yAxis"
        type: Vector
        nillable: true
      - name: "zAxis"
        type: Vector
        nillable: true
      - name: "origin"
        type: Vector
        nillable: true
    - arguments:
      - name: "m11"
        type: number
      - name: "m12"
        type: number
      - name: "m13"
        type: number
      - name: "m14"
        type: number
      - name: "m21"
        type: number
      - name: "m22"
        type: number
      - name: "m23"
        type: number
      - name: "m24"
        type: number
      - name: "m31"
        type: number
      - name: "m32"
        type: number
      - name: "m33"
        type: number
      - name: "m34"
        type: number
  returns:
    - type: Matrix3x4
      description: "The created 3x4 matrix."
---

<div class="lua__search__keywords">
Matrices.Create3x4 &#x2013; Creates a new 3x4 matrix.
</div>
