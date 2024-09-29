---
template: lua-class-function.html
title: ShouldToggle
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: ShouldToggle
  realm: server
  description: "Should toggle."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "useType"
    type: enumeration/USE_TYPE
  - position: 3
    name: "currentState"
    type: number
  returns:
    - type: integer
      description: "The toggle state"
---

<div class="lua__search__keywords">
Entity:ShouldToggle &#x2013; Should toggle.
</div>
