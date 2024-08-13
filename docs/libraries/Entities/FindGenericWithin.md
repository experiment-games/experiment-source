---
template: lua-library-function.html
title: FindGenericWithin
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entities
  function: FindGenericWithin
  realm: server
  description: "Finds an entity by its generic name within a radius"
  
  arguments:
    - name: "generic"
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
Entities.FindGenericWithin &#x2013; Finds an entity by its generic name within a radius
</div>
