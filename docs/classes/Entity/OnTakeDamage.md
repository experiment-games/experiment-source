---
template: lua-class-function.html
title: OnTakeDamage
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: OnTakeDamage
  realm: server
  description: "On take damage."
  
  arguments:
    - name: "damageInfo"
      type: DamageInfo
  returns:
    - type: integer
      description: "0 if no damage is taken, or it was fatal. 1 Otherwise"
---

<div class="lua__search__keywords">
Entity:OnTakeDamage &#x2013; On take damage.
</div>