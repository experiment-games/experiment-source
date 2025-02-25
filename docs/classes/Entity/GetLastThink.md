---
template: lua-class-function.html
title: GetLastThink
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: GetLastThink
  realm: shared
  description: "Get last think."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "name"
    type: string
    default: "nullptr"
  returns:
    - type: number
      description: "The last think."
---

<div class="lua__search__keywords">
Entity:GetLastThink &#x2013; Get last think.
</div>
