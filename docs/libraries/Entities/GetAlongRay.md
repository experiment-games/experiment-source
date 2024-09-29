---
template: lua-library-function.html
title: GetAlongRay
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entities
  function: GetAlongRay
  realm: shared
  description: "Finds all entities along the given ray."
  
  arguments:
  - position: 1
    name: "start"
    type: Vector
  - position: 2
    name: "end"
    type: Vector
  - position: 3
    name: "mins"
    type: Vector
  - position: 4
    name: "maxs"
    type: Vector
  returns:
    - type: table
      description: "A table of entities found."
    - type: integer
      description: "The number of entities found."
---

<div class="lua__search__keywords">
Entities.GetAlongRay &#x2013; Finds all entities along the given ray.
</div>
