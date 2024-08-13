---
template: lua-library-function.html
title: FindByNameWithin
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entities
  function: FindByNameWithin
  realm: server
  description: "Finds an entity by its name within a radius"
  
  arguments:
    - name: "name"
      type: string
    - name: "position"
      type: Vector
    - name: "maxDistance"
      type: number
    - name: "startEntity"
      type: CBaseEntity
      default: "NULL"
  returns:
    - type: Entity
      description: "The entity found, or NULL if not found."
---

<div class="lua__search__keywords">
Entities.FindByNameWithin &#x2013; Finds an entity by its name within a radius
</div>
