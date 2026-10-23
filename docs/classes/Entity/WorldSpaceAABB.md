---
template: lua-class-function.html
title: WorldSpaceAABB
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: WorldSpaceAABB
  realm: shared
  description: "Returns the world axis-aligned bounding box of the entity as two vectors."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  returns:
    - type: Vector
      description: "The minimum world position"
    - type: Vector
      description: "The maximum world position"
---

<div class="lua__search__keywords">
Entity:WorldSpaceAABB &#x2013; Returns the world axis-aligned bounding box of the entity as two vectors.
</div>
