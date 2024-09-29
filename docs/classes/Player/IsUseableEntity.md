---
template: lua-class-function.html
title: IsUseableEntity
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Player
  function: IsUseableEntity
  realm: shared
  description: "Check if the player can use an entity."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "entity"
    type: Entity
  - position: 3
    name: "use type"
    type: number
  returns:
    - type: boolean
      description: "Whether the player can use the entity."
---

<div class="lua__search__keywords">
Player:IsUseableEntity &#x2013; Check if the player can use an entity.
</div>
