---
template: lua-class-function.html
title: SetPoseParameter
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: SetPoseParameter
  realm: shared
  description: "Set the pose parameter."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    types:
      - name: "poseParameter"
        type: number
      - name: "poseParameter"
        type: string
  - position: 3
    types:
      - name: "value"
        type: number
      - name: "value"
        type: number
  returns:
    - type: number
      description: "The pose parameter"
---

<div class="lua__search__keywords">
Entity:SetPoseParameter &#x2013; Set the pose parameter.
</div>
