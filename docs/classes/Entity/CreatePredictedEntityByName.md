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
  library: Entity
  function: CreatePredictedEntityByName
  realm: shared
  description: "Create predicted entity by name."
  
  arguments:
    - name: "className"
      type: string
    - name: "module"
      type: string
    - name: "line"
      type: number
    - name: "shouldPersist"
      type: boolean
      default: "false"
  returns:
    - type: entity
      description: "The predicted entity."
---

<div class="lua__search__keywords">
Entity:CreatePredictedEntityByName &#x2013; Create predicted entity by name.
</div>
