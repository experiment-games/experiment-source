---
template: lua-class-function.html
title: SetNotSolid
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: SetNotSolid
  realm: shared
  description: "Sets whether the entity should collide with anything. Keeps the solid type intact and only toggles the FSOLID_NOT_SOLID flag, so the solid type still reads the same afterwards."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "notSolid"
    type: boolean
  returns:
    
---

<div class="lua__search__keywords">
Entity:SetNotSolid &#x2013; Sets whether the entity should collide with anything. Keeps the solid type intact and only toggles the FSOLID_NOT_SOLID flag, so the solid type still reads the same afterwards.
</div>
