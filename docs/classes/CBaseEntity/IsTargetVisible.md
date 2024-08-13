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
  library: CBaseEntity
  function: IsTargetVisible
  realm: server
  description: "Returns true if a line can be traced from the caller's eyes to the target"
  
  argumentSets:
    - arguments:
      - name: "position"
        type: Vector
      - name: "mask"
        type: integer
        default: "MASK_BLOCKLOS"
    - arguments:
      - name: "target"
        type: CBaseEntity
      - name: "mask"
        type: integer
        default: "MASK_BLOCKLOS"
  returns:
    - type: boolean
      description: "true if visible, false otherwise."
---

<div class="lua__search__keywords">
CBaseEntity:IsTargetVisible &#x2013; Returns true if a line can be traced from the caller's eyes to the target
</div>
