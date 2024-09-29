---
template: lua-class-function.html
title: ShouldAttractAutoAim
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: ShouldAttractAutoAim
  realm: server
  description: "Checks if the autoaim should be attracted towards the given entity."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "aimingEntity"
    type: Entity
  returns:
    - type: boolean
      description: "true if should attract, false otherwise."
---

<div class="lua__search__keywords">
Entity:ShouldAttractAutoAim &#x2013; Checks if the autoaim should be attracted towards the given entity.
</div>
