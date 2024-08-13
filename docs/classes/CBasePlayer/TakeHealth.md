---
template: lua-class-function.html
title: TakeHealth
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: CBasePlayer
  function: TakeHealth
  realm: server
  description: "Take health from the player."
  
  arguments:
    - name: "health"
      type: number
    - name: "damageType"
      type: integer
  returns:
    - type: integer
      description: "The amount of health actually taken."
---

<div class="lua__search__keywords">
CBasePlayer:TakeHealth &#x2013; Take health from the player.
</div>
