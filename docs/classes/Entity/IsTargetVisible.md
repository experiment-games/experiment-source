---
template: lua-class-function.html
title: IsTargetVisible
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: IsTargetVisible
  realm: server
  description: "Returns true if a line can be traced from the caller's eyes to the target"
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    types:
      - name: "position"
        type: Vector
      - name: "target"
        type: Entity
  - position: 3
    types:
      - name: "mask"
        type: integer
    default: "MASK_BLOCKLOS"
      - name: "mask"
        type: integer
    default: "MASK_BLOCKLOS"
  returns:
    - type: boolean
      description: "true if visible, false otherwise."
---

<div class="lua__search__keywords">
Entity:IsTargetVisible &#x2013; Returns true if a line can be traced from the caller's eyes to the target
</div>
