---
template: lua-class-function.html
title: GetAimEntOrigin
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: GetAimEntOrigin
  realm: client
  description: "Get the aim entity origin."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "attachedTo"
    type: Entity
  returns:
    - type: Vector
      description: "The aim entity origin"
    - type: Angle
      description: "The aim entity angles"
---

<div class="lua__search__keywords">
Entity:GetAimEntOrigin &#x2013; Get the aim entity origin.
</div>
