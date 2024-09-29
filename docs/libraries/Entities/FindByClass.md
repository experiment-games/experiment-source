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
  - position: 1
    name: "className"
    type: string
  - position: 2
    name: "startEntity"
    type: Entity
    default: "NULL"
  returns:
    - type: Entity
      description: "The entity found, or NULL if not found."
---

<div class="lua__search__keywords">
Entities.FindByClass &#x2013; Finds an entity by its class name
</div>
