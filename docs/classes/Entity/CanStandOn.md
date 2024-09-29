---
template: lua-class-function.html
title: CanStandOn
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: CanStandOn
  realm: server
  description: "Whether this entity can stand on the given surface entity."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "surfaceEntity"
    type: Entity
  returns:
    - type: boolean
      description: "true if can stand on, false otherwise."
---

<div class="lua__search__keywords">
Entity:CanStandOn &#x2013; Whether this entity can stand on the given surface entity.
</div>
