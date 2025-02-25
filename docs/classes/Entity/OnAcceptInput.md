---
template: lua-class-function.html
title: OnAcceptInput
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: OnAcceptInput
  realm: server
  description: "Called when the entity accepted input"
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "input"
    type: string
  - position: 3
    name: "activator"
    type: Entity
  - position: 4
    name: "caller"
    type: Entity
  - position: 5
    name: "outputID"
    type: number
  returns:
    - type: boolean
      description: "true if accepted, false otherwise."
---

<div class="lua__search__keywords">
Entity:OnAcceptInput &#x2013; Called when the entity accepted input
</div>
