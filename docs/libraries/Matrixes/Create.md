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
  library: Matrixes
  function: Create
  realm: shared
  description: "Creates a new matrix."
  
  arguments:
  - position: 1
    types:
      - name: "m11"
        type: number
      - name: "Matrix3x4"
        type: Matrix3x4
    nillable: true
      - name: "xAxis"
        type: Vector
    nillable: true
  - position: 2
    types:
      - name: "m12"
        type: number
      - name: "yAxis"
        type: Vector
    nillable: true
  - position: 3
    types:
      - name: "m13"
        type: number
      - name: "zAxis"
        type: Vector
    nillable: true
  - position: 4
    name: "m14"
    type: number
  - position: 5
    name: "m21"
    type: number
  - position: 6
    name: "m22"
    type: number
  - position: 7
    name: "m23"
    type: number
  - position: 8
    name: "m24"
    type: number
  - position: 9
    name: "m31"
    type: number
  - position: 10
    name: "m32"
    type: number
  - position: 11
    name: "m33"
    type: number
  - position: 12
    name: "m34"
    type: number
  - position: 13
    name: "m41"
    type: number
  - position: 14
    name: "m42"
    type: number
  - position: 15
    name: "m43"
    type: number
  - position: 16
    name: "m44"
    type: number
  returns:
    - type: Matrix
      description: "The created matrix."
---

<div class="lua__search__keywords">
Matrixes.Create &#x2013; Creates a new matrix.
</div>
