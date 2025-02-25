---
template: lua-class-function.html
title: GetBasisVectors
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Matrix
  function: GetBasisVectors
  realm: shared
  description: "Gets the basis vectors of the matrix."
  
  arguments:
  - position: 1
    name: "Matrix"
    type: Matrix
  returns:
    - type: Vector
      description: "The forward vector."
    - type: Vector
      description: "The left vector."
    - type: Vector
      description: "The up vector."
---

<div class="lua__search__keywords">
Matrix:GetBasisVectors &#x2013; Gets the basis vectors of the matrix.
</div>
