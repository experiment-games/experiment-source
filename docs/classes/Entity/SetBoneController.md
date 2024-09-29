---
template: lua-class-function.html
title: SetBoneController
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: SetBoneController
  realm: shared
  description: "Set the value for the specified bone controller."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "boneController"
    type: number
  - position: 3
    name: "value"
    type: number
  returns:
    - type: number
      description: "The bone controller value, clamped between acceptable values"
---

<div class="lua__search__keywords">
Entity:SetBoneController &#x2013; Set the value for the specified bone controller.
</div>
