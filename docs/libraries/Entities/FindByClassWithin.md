---
template: lua-library-function.html
title: FindByClassWithin
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entities
  function: FindByClassWithin
  realm: server
  description: "Finds an entity by its class name within a radius"
  
  arguments:
  - position: 1
    name: "className"
    type: string
  - position: 2
    name: "position"
    type: Vector
  - position: 3
    name: "maxDistance"
    type: number
  - position: 4
    name: "startEntity"
    type: Entity
    default: "NULL"
  returns:
    - type: Entity
      description: "The entity found, or NULL if not found."
---

<div class="lua__search__keywords">
Entities.FindByClassWithin &#x2013; Finds an entity by its class name within a radius
</div>
