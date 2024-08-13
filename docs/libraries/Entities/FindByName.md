---
template: lua-library-function.html
title: FindByName
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entities
  function: FindByName
  realm: server
  description: "Finds an entity by its name"
  
  arguments:
    - name: "name"
      type: string
    - name: "startEntity"
      type: CBaseEntity
      default: "NULL"
  returns:
    - type: Entity
      description: "The entity found, or NULL if not found."
---

<div class="lua__search__keywords">
Entities.FindByName &#x2013; Finds an entity by its name
</div>
