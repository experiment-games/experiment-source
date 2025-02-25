---
template: lua-library-function.html
title: FindByNameNearest
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entities
  function: FindByNameNearest
  realm: server
  description: "Finds the nearest entity by its name"
  
  arguments:
  - position: 1
    name: "name"
    type: string
  - position: 2
    name: "position"
    type: Vector
  - position: 3
    name: "maxDistance"
    type: number
  returns:
    - type: Entity
      description: "The entity found, or NULL if not found."
---

<div class="lua__search__keywords">
Entities.FindByNameNearest &#x2013; Finds the nearest entity by its name
</div>
