---
template: lua-library-function.html
title: Create
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Materials
  function: Create
  realm: client
  description: "Creates a material"
  
  arguments:
  - position: 1
    name: "name"
    type: string
  - position: 2
    name: "shaderName"
    type: string
  - position: 3
    name: "materialData"
    type: table
    nillable: true
  returns:
    - type: Material
      description: "The material (or nil if it could not be created)"
---

<div class="lua__search__keywords">
Materials.Create &#x2013; Creates a material
</div>
