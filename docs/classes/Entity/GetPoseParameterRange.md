---
template: lua-class-function.html
title: GetPoseParameterRange
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: GetPoseParameterRange
  realm: shared
  description: "Get the pose parameter range."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "poseParameter"
    type: number
  returns:
    - type: boolean
      description: "Whether the operation was successful"
    - type: number
      description: "The minimum value"
    - type: number
      description: "The maximum value"
---

<div class="lua__search__keywords">
Entity:GetPoseParameterRange &#x2013; Get the pose parameter range.
</div>
