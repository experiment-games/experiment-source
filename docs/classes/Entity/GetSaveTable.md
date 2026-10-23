---
template: lua-class-function.html
title: GetSaveTable
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: GetSaveTable
  realm: shared
  description: "Gets all data that would be saved for the entity. Not implemented for: FIELD_CLASSPTR, FIELD_EDICT, FIELD_CUSTOM"
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "showAll"
    type: boolean
    default: "false"
  returns:
    
---

<div class="lua__search__keywords">
Entity:GetSaveTable &#x2013; Gets all data that would be saved for the entity. Not implemented for: FIELD_CLASSPTR, FIELD_EDICT, FIELD_CUSTOM
</div>
