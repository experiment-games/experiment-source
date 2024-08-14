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
    - name: "position"
      type: Vector
    - name: "radius"
      type: number
    - name: "startEntity"
      type: Entity
      default: "NULL"
  returns:
    - type: Entity
      description: "The entity found, or NULL if not found."
---

<div class="lua__search__keywords">
Entities.FindInSphere &#x2013; Finds an entity within a radius
</div>
