---
template: lua-class-function.html
title: ClampPunchAngleOffset
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Player
  function: ClampPunchAngleOffset
  realm: server
  description: "Adds angleToClamp with the player's punch angle and clamps it between -clampTo and clampTo."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "start"
    type: Angle
  - position: 3
    name: "clampTo"
    type: Angle
  returns:
    
---

<div class="lua__search__keywords">
Player:ClampPunchAngleOffset &#x2013; Adds angleToClamp with the player's punch angle and clamps it between -clampTo and clampTo.
</div>
