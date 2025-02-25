---
template: lua-class-function.html
title: __index
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: __index
  realm: shared
  description: "Metamethod that is called when a non existing field is indexed"
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "field"
    type: string
  returns:
    - type: any
      description: "The value of the field"
---

<div class="lua__search__keywords">
Entity:__index &#x2013; Metamethod that is called when a non existing field is indexed
</div>
