---
template: lua-class-function.html
title: SetDsp
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Player
  function: SetDsp
  realm: shared
  description: "Set the player's DSP."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "dspEffectId"
    type: integer
  - position: 3
    name: "fastReset"
    type: boolean
    default: "false"
  returns:
    
---

<div class="lua__search__keywords">
Player:SetDsp &#x2013; Set the player's DSP.
</div>
