---
template: lua-class-function.html
title: DropToFloor
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: DropToFloor
  realm: server
  description: "Drops the entity to the floor."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "mask"
    type: integer
    default: "MASK_SOLID"
  - position: 3
    name: "ignore"
    type: Entity
    default: "NULL"
  returns:
    - type: integer
      description: "-1 if the floor to drop to isn't valid. 0 if nothing changed. 1 if the entity dropped to the floor"
---

<div class="lua__search__keywords">
Entity:DropToFloor &#x2013; Drops the entity to the floor.
</div>
