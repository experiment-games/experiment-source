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
  - position: 1
    name: "generic"
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
Entities.FindGeneric &#x2013; Finds an entity by its generic name
</div>
