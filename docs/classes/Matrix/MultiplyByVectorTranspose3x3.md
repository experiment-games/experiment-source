---
template: lua-class-function.html
title: MultiplyByVectorTranspose3x3
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Matrix
  function: MultiplyByVectorTranspose3x3
  realm: shared
  description: "Apply the inverse (transposed) rotation (only works on pure rotation matrix)."
  
  arguments:
  - position: 1
    name: "Matrix"
    type: Matrix
  - position: 2
    name: "vector"
    type: Vector
  returns:
    - type: Vector
      description: "The multiplied vector."
---

<div class="lua__search__keywords">
Matrix:MultiplyByVectorTranspose3x3 &#x2013; Apply the inverse (transposed) rotation (only works on pure rotation matrix).
</div>
