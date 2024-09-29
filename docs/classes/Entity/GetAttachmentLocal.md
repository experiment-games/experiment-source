---
template: lua-class-function.html
title: GetAttachmentLocal
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: GetAttachmentLocal
  realm: shared
  description: "Get the local attachment."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "attachment"
    type: number
  returns:
    - type: boolean
      description: "Whether the operation was successful"
    - type: Vector
      description: "The origin"
    - type: Angle
      description: "The angles"
---

<div class="lua__search__keywords">
Entity:GetAttachmentLocal &#x2013; Get the local attachment.
</div>
