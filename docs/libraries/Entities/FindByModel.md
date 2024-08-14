---
template: lua-library-function.html
title: FindByModel
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entities
  function: FindByModel
  realm: server
  description: "Finds an entity by its model name"
  
  arguments:
    - name: "modelName"
      type: string
    - name: "startEntity"
      type: Entity
      default: "NULL"
  returns:
    - type: Entity
      description: "The entity found, or NULL if not found."
---

<div class="lua__search__keywords">
Entities.FindByModel &#x2013; Finds an entity by its model name
</div>
