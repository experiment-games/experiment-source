---
template: lua-class-function.html
title: DispatchMuzzleEffect
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: DispatchMuzzleEffect
  realm: client
  description: "Dispatch the muzzle effect."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "options"
    type: string
  - position: 3
    name: "isFirstPerson"
    type: boolean
  returns:
    - type: boolean
      description: "Whether the operation was successful"
---

<div class="lua__search__keywords">
Entity:DispatchMuzzleEffect &#x2013; Dispatch the muzzle effect.
</div>
