---
template: lua-class-function.html
title: CreatePredictedEntityByName
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entities
  function: CreatePredictedEntityByName
  realm: shared
  description: "Create predicted entity by name."
  
  arguments:
  - position: 1
    name: "className"
    type: string
  - position: 2
    name: "module"
    type: string
  - position: 3
    name: "line"
    type: number
  - position: 4
    name: "shouldPersist"
    type: boolean
    default: "false"
  returns:
    - type: Entity
      description: "The predicted entity."
---

<div class="lua__search__keywords">
Entities:CreatePredictedEntityByName &#x2013; Create predicted entity by name.
</div>
