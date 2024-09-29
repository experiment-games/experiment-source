---
template: lua-class-function.html
title: __eq
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: __eq
  realm: shared
  description: "Metamethod that is called when comparing two entities"
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "other"
    type: Entity
  returns:
    - type: boolean
      description: "True if entities are equal."
---

<div class="lua__search__keywords">
Entity:__eq &#x2013; Metamethod that is called when comparing two entities
</div>
