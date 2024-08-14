---
template: lua-library-function.html
title: FindClientInPvs
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entities
  function: FindClientInPvs
  realm: server
  description: "Finds a client in the PVS"
  
  arguments:
    - name: "minimumVector"
      type: Vector
    - name: "maximumVector"
      type: Vector
  returns:
    - type: Entity
      description: "The entity found, or NULL if not found."
---

<div class="lua__search__keywords">
Entities.FindClientInPvs &#x2013; Finds a client in the PVS
</div>
