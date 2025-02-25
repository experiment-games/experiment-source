---
template: lua-class-function.html
title: Ignite
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: Ignite
  realm: server
  description: "Ignite the entity."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "lifetimeInSeconds"
    type: number
  - position: 3
    name: "spreadRadius"
    type: number
    default: "0.0f"
  returns:
    
---

<div class="lua__search__keywords">
Entity:Ignite &#x2013; Ignite the entity.
</div>
