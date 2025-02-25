---
template: lua-class-function.html
title: IsLocked
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Weapon
  function: IsLocked
  realm: shared
  description: "Check if the weapon is locked."
  
  arguments:
  - position: 1
    name: "entity"
    type: Weapon
  - position: 2
    name: "asker"
    type: Entity
  returns:
    - type: boolean
      description: "Whether the weapon is locked."
---

<div class="lua__search__keywords">
Weapon:IsLocked &#x2013; Check if the weapon is locked.
</div>
