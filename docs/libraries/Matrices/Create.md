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
  library: Matrices
  function: Create
  realm: shared
  description: "Creates a new matrix."
  
  argumentSets:
    - arguments:
      - name: "Matrix3x4"
        type: Matrix3x4
        nillable: true
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
      - name: "m41"
        type: number
      - name: "m42"
        type: number
      - name: "m43"
        type: number
      - name: "m44"
        type: number
  returns:
    - type: Matrix
      description: "The created matrix."
---

<div class="lua__search__keywords">
Matrices.Create &#x2013; Creates a new matrix.
</div>
