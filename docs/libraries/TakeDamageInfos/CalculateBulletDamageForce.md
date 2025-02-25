---
template: lua-library-function.html
title: CalculateBulletDamageForce
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: TakeDamageInfos
  function: CalculateBulletDamageForce
  realm: shared
  description: "Calculates the bullet damage force"
  
  arguments:
  - position: 1
    name: "damageInfo"
    type: DamageInfo
  - position: 2
    name: "bulletType"
    type: number
  - position: 3
    name: "damageDirection"
    type: Vector
  - position: 4
    name: "damagePosition"
    type: Vector
  - position: 5
    name: "scale"
    type: number
    default: "1.0"
  returns:
    
---

<div class="lua__search__keywords">
TakeDamageInfos.CalculateBulletDamageForce &#x2013; Calculates the bullet damage force
</div>
