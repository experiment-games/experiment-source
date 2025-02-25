---
template: lua-library-function.html
title: FindInSphere
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entities
  function: FindInSphere
  realm: server
  description: "Finds an entity within a radius"
  
  arguments:
  - position: 1
    name: "position"
    type: Vector
  - position: 2
    name: "radius"
    type: number
  - position: 3
    name: "startEntity"
    type: Entity
    default: "NULL"
  returns:
    - type: Entity
      description: "The entity found, or NULL if not found."
---

<div class="lua__search__keywords">
Entities.FindInSphere &#x2013; Finds an entity within a radius
</div>
