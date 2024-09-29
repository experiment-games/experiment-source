---
template: lua-class-function.html
title: GetPoseParameter
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: GetPoseParameter
  realm: shared
  description: "Get the pose parameter."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    types:
      - name: "poseParameter"
        type: number
      - name: "poseParameterName"
        type: string
  returns:
    - type: number
      description: "The pose parameter"
---

<div class="lua__search__keywords">
Entity:GetPoseParameter &#x2013; Get the pose parameter.
</div>
