---
template: lua-library-function.html
title: FindProcedural
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entities
  function: FindProcedural
  realm: server
  description: "Finds an entity by its procedural name"
  
  arguments:
    - name: "procedural"
      type: string
    - name: "startEntity"
      type: Entity
      default: "NULL"
  returns:
    - type: Entity
      description: "The entity found, or NULL if not found."
---

<div class="lua__search__keywords">
Entities.FindProcedural &#x2013; Finds an entity by its procedural name
</div>
