---
template: lua-class-function.html
title: SetMoveType
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: SetMoveType
  realm: shared
  description: "Set move type."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "type"
    type: enumeration/MOVE_TYPE
  - position: 3
    name: "collide"
    type: enumeration/MOVE_COLLIDE
    default: "MOVECOLLIDE_DEFAULT"
  returns:
    
---

<div class="lua__search__keywords">
Entity:SetMoveType &#x2013; Set move type.
</div>
