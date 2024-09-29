---
template: lua-library-function.html
title: Find
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Materials
  function: Find
  realm: shared
  description: "Finds a material"
  
  arguments:
  - position: 1
    name: "name"
    type: string
  - position: 2
    name: "parameters"
    type: string
    default: "\"\""
  returns:
    - type: Material
      description: "The material"
---

<div class="lua__search__keywords">
Materials.Find &#x2013; Finds a material
</div>
