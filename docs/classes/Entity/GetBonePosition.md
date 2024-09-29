---
template: lua-class-function.html
title: GetBonePosition
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: GetBonePosition
  realm: shared
  description: "Get the bone position."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "bone"
    type: number
  returns:
    - type: Vector
      description: "The origin"
    - type: Angle
      description: "The angles"
---

<div class="lua__search__keywords">
Entity:GetBonePosition &#x2013; Get the bone position.
</div>
