---
template: lua-library-function.html
title: FindGenericNearest
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entities
  function: FindGenericNearest
  realm: server
  description: "Finds the nearest entity by its generic name"
  
  arguments:
    - name: "generic"
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
Entities.FindGenericNearest &#x2013; Finds the nearest entity by its generic name
</div>
