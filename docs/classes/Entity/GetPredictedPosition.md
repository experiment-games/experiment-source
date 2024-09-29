---
template: lua-class-function.html
title: GetPredictedPosition
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: GetPredictedPosition
  realm: server
  description: "Get the predicted postion of an entity of a certain number of seconds. Use this function with caution, it has great potential for annoying the player, especially if used for target firing predition"
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "timeDelta"
    type: number
  returns:
    - type: Vector
      description: "The predicted position of the entity"
---

<div class="lua__search__keywords">
Entity:GetPredictedPosition &#x2013; Get the predicted postion of an entity of a certain number of seconds. Use this function with caution, it has great potential for annoying the player, especially if used for target firing predition
</div>
