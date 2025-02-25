---
template: lua-class-function.html
title: TakeHealth
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: TakeHealth
  realm: server
  description: "Take health."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "health"
    type: number
  - position: 3
    name: "bitsDamageType"
    type: number
  returns:
    - type: integer
      description: "The health actually taken"
---

<div class="lua__search__keywords">
Entity:TakeHealth &#x2013; Take health.
</div>
