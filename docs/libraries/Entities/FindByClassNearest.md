---
template: lua-library-function.html
title: FindByClassNearest
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entities
  function: FindByClassNearest
  realm: server
  description: "Finds the nearest entity by its class name"
  
  arguments:
    - name: "className"
      type: string
    - name: "position"
      type: Vector
    - name: "maxDistance"
      type: number
  returns:
    - type: Entity
      description: "The entity found, or NULL if not found."
---

<div class="lua__search__keywords">
Entities.FindByClassNearest &#x2013; Finds the nearest entity by its class name
</div>
