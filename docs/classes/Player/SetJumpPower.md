---
template: lua-class-function.html
title: SetJumpPower
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Player
  function: SetJumpPower
  realm: shared
  description: "Set the player's jump power. Is multiplied by jumpFactor of the surface the player is standing on. Supposedly (according to a Valve comment inside the SDK) 160 power equals about 21 units jump height."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "power"
    type: number
  returns:
    
---

<div class="lua__search__keywords">
Player:SetJumpPower &#x2013; Set the player's jump power. Is multiplied by jumpFactor of the surface the player is standing on. Supposedly (according to a Valve comment inside the SDK) 160 power equals about 21 units jump height.
</div>
