---
template: lua-library-function.html
title: FindGeneric
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entities
  function: FindGeneric
  realm: server
  description: "Finds an entity by its generic name"
  
  arguments:
    - name: "generic"
      type: string
    - name: "startEntity"
      type: CBaseEntity
      default: "NULL"
  returns:
    - type: Entity
      description: "The entity found, or NULL if not found."
---

<div class="lua__search__keywords">
Entities.FindGeneric &#x2013; Finds an entity by its generic name
</div>
