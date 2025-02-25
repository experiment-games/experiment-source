---
template: lua-library-function.html
title: GuessDamageForce
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: TakeDamageInfos
  function: GuessDamageForce
  realm: shared
  description: "Guesses the damage force"
  
  arguments:
  - position: 1
    name: "damageInfo"
    type: DamageInfo
  - position: 2
    name: "damageDirection"
    type: Vector
  - position: 3
    name: "damagePosition"
    type: Vector
  - position: 4
    name: "scale"
    type: number
    default: "1.0"
  returns:
    
---

<div class="lua__search__keywords">
TakeDamageInfos.GuessDamageForce &#x2013; Guesses the damage force
</div>
