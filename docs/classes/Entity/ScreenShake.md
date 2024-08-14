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
    - name: "center"
      type: Vector
    - name: "amplitude"
      type: number
    - name: "frequency"
      type: number
    - name: "duration"
      type: number
    - name: "radius"
      type: number
    - name: "shakeCommand"
      type: number
      default: "SHAKE_START"
    - name: "shouldShakePlayersInAir"
      type: boolean
      default: "false"
  returns:
    
---

<div class="lua__search__keywords">
Entity:ScreenShake &#x2013; Shake an object and all players on or near it.
</div>
