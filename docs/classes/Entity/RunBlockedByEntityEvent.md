---
template: lua-class-function.html
title: RunBlockedByEntityEvent
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: RunBlockedByEntityEvent
  realm: server
  description: "Dispatches blocked events to this entity's blocked handler, set via SetBlocked."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "blockingEntity"
    type: Entity
  returns:
    
---

<div class="lua__search__keywords">
Entity:RunBlockedByEntityEvent &#x2013; Dispatches blocked events to this entity's blocked handler, set via SetBlocked.
</div>
