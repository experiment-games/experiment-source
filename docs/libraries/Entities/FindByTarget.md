---
template: lua-library-function.html
title: FindByTarget
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entities
  function: FindByTarget
  realm: server
  description: "Finds an entity by its target name"
  
  arguments:
    - name: "target"
      type: string
    - name: "startEntity"
      type: CBaseEntity
      default: "NULL"
  returns:
    - type: Entity
      description: "The entity found, or NULL if not found."
---

<div class="lua__search__keywords">
Entities.FindByTarget &#x2013; Finds an entity by its target name
</div>
