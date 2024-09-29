---
template: lua-class-function.html
title: Use
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: Use
  realm: server
  description: "Dispatches use events to this entity's use handler, set via SetUse."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "activator"
    type: Entity
  - position: 3
    name: "caller"
    type: Entity
  - position: 4
    name: "useType"
    type: enumeration/USE_TYPE
  - position: 5
    name: "value"
    type: number
  returns:
    
---

<div class="lua__search__keywords">
Entity:Use &#x2013; Dispatches use events to this entity's use handler, set via SetUse.
</div>
