---
template: lua-class-function.html
title: ComputeEntitySpaceHitboxSurroundingBox
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: ComputeEntitySpaceHitboxSurroundingBox
  realm: shared
  description: "Compute the entity space hitbox surrounding box."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  returns:
    - type: boolean
      description: "Whether the operation was successful"
    - type: Vector
      description: "The world mins"
    - type: Vector
      description: "The world maxs"
---

<div class="lua__search__keywords">
Entity:ComputeEntitySpaceHitboxSurroundingBox &#x2013; Compute the entity space hitbox surrounding box.
</div>
