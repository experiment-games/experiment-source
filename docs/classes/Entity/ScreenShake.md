---
template: lua-class-function.html
title: ScreenShake
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: ScreenShake
  realm: server
  description: "Shake an object and all players on or near it."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "center"
    type: Vector
  - position: 3
    name: "amplitude"
    type: number
  - position: 4
    name: "frequency"
    type: number
  - position: 5
    name: "duration"
    type: number
  - position: 6
    name: "radius"
    type: number
  - position: 7
    name: "shakeCommand"
    type: enumeration/SHAKE_COMMAND
    default: "SHAKE_START"
  - position: 8
    name: "shouldShakePlayersInAir"
    type: boolean
    default: "false"
  returns:
    
---

<div class="lua__search__keywords">
Entity:ScreenShake &#x2013; Shake an object and all players on or near it.
</div>
