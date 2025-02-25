---
template: lua-class-function.html
title: IsTriggered
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: IsTriggered
  realm: server
  description: "Is triggered."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "activator"
    type: Entity
  returns:
    - type: boolean
      description: "true if triggered, false otherwise."
---

<div class="lua__search__keywords">
Entity:IsTriggered &#x2013; Is triggered.
</div>
