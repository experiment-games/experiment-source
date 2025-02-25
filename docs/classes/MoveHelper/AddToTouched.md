---
template: lua-class-function.html
title: AddToTouched
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: MoveHelper
  function: AddToTouched
  realm: shared
  description: "Adds the trace result to touch list, if contact is not already in list"
  
  arguments:
  - position: 1
    name: "moveHelper"
    type: MoveHelper
  - position: 2
    name: "trace"
    type: Trace
  - position: 3
    name: "impactVelocity"
    type: Vector
  returns:
    - type: boolean
      description: "Returns true if the contact was added to the touch list, false if it already exists"
---

<div class="lua__search__keywords">
MoveHelper:AddToTouched &#x2013; Adds the trace result to touch list, if contact is not already in list
</div>
