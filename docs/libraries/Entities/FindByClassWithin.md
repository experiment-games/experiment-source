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
    - name: "className"
      type: string
    - name: "position"
      type: Vector
    - name: "maxDistance"
      type: number
    - name: "startEntity"
      type: Entity
      default: "NULL"
  returns:
    - type: Entity
      description: "The entity found, or NULL if not found."
---

<div class="lua__search__keywords">
Entities.FindByClassWithin &#x2013; Finds an entity by its class name within a radius
</div>
