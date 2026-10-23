---
template: lua-class-function.html
title: GetInternalVariable
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: GetInternalVariable
  realm: shared
  description: "Gets an internal variable of the entity."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "variableName"
    type: string
  returns:
    - type: any
      description: "The value of the internal variable."
---

<div class="lua__search__keywords">
Entity:GetInternalVariable &#x2013; Gets an internal variable of the entity.
</div>
