---
template: lua-library-function.html
title: FindByClass
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entities
  function: FindByClass
  realm: server
  description: "Finds an entity by its class name"
  
  arguments:
    - name: "className"
      type: string
    - name: "startEntity"
      type: CBaseEntity
      default: "NULL"
  returns:
    - type: Entity
      description: "The entity found, or NULL if not found."
---

<div class="lua__search__keywords">
Entities.FindByClass &#x2013; Finds an entity by its class name
</div>
