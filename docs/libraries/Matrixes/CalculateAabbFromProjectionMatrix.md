---
template: lua-library-function.html
title: CalculateAabbFromProjectionMatrix
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Matrixes
  function: CalculateAabbFromProjectionMatrix
  realm: shared
  description: "Calculates an AABB from a projection matrix."
  
  arguments:
  - position: 1
    name: "Matrix"
    type: Matrix
  returns:
    - type: Vector
      description: "The minimum vector."
    - type: Vector
      description: "The maximum vector."
---

<div class="lua__search__keywords">
Matrixes.CalculateAabbFromProjectionMatrix &#x2013; Calculates an AABB from a projection matrix.
</div>
