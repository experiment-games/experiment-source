---
template: lua-library-function.html
title: GetInPvs
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entities
  function: GetInPvs
  realm: server
  description: "Goes through the entities and find the ones that are in the PVS of the provided vector."
  
  arguments:
  - position: 1
    name: "viewOrigin"
    type: Vector
  returns:
    - type: table
      description: "A table of entities found."
    - type: integer
      description: "The number of entities found."
---

<div class="lua__search__keywords">
Entities.GetInPvs &#x2013; Goes through the entities and find the ones that are in the PVS of the provided vector.
</div>
