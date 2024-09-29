---
template: lua-class-function.html
title: CanBeHitByMeleeAttack
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: CanBeHitByMeleeAttack
  realm: server
  description: "Can be hit by melee attack."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "attacker"
    type: Entity
  returns:
    - type: boolean
      description: "true if can be hit, false otherwise."
---

<div class="lua__search__keywords">
Entity:CanBeHitByMeleeAttack &#x2013; Can be hit by melee attack.
</div>
