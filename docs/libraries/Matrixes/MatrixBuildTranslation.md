---
template: lua-library-function.html
title: MatrixBuildTranslation
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Matrixes
  function: MatrixBuildTranslation
  realm: shared
  description: "Builds a translation matrix."
  
  arguments:
  - position: 1
    name: "Matrix"
    type: Matrix
  - position: 2
    types:
      - name: "x"
        type: number
      - name: "translation"
        type: Vector
  - position: 3
    name: "y"
    type: number
  - position: 4
    name: "z"
    type: number
  returns:
    
---

<div class="lua__search__keywords">
Matrixes.MatrixBuildTranslation &#x2013; Builds a translation matrix.
</div>
