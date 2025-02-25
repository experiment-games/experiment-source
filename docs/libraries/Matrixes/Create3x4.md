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
  library: Matrixes
  function: Create3x4
  realm: shared
  description: "Creates a new 3x4 matrix."
  
  arguments:
  - position: 1
    types:
      - name: "m11"
        type: number
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
    types:
      - name: "m14"
        type: number
      - name: "origin"
        type: Vector
    nillable: true
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
  returns:
    - type: Matrix3x4
      description: "The created 3x4 matrix."
---

<div class="lua__search__keywords">
Matrixes.Create3x4 &#x2013; Creates a new 3x4 matrix.
</div>
