---
template: lua-class-function.html
title: HasTarget
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: HasTarget
  realm: server
  description: "Has target."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "target"
    type: string
  returns:
    - type: boolean
      description: "true if has target, false otherwise."
---

<div class="lua__search__keywords">
Entity:HasTarget &#x2013; Has target.
</div>
