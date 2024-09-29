---
template: lua-class-function.html
title: SetNextPrimaryFire
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Weapon
  function: SetNextPrimaryFire
  realm: shared
  description: "Set next time before the player can fire the primary attack. E.g: `Engines.GetCurrentTime() + 2` for two seconds from now."
  
  arguments:
  - position: 1
    name: "entity"
    type: Weapon
  - position: 2
    name: "absoluteCurTime"
    type: number
  returns:
    
---

<div class="lua__search__keywords">
Weapon:SetNextPrimaryFire &#x2013; Set next time before the player can fire the primary attack. E.g: `Engines.GetCurrentTime() + 2` for two seconds from now.
</div>
