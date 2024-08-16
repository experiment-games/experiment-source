---
template: lua-hook.html
title: GetFallDamage
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  function: GetFallDamage
  realm: shared
  description: "Called when a player falls and fall damage is calculated."
  arguments:
    - name: "player"
      description: ""
      type: Entity
    - name: "flFallVelocity"
      description: ""
      type: number
  returns:
    - type: number
      description: "override fall damage amount"
---

<div class="lua__search__keywords">
hook GetFallDamage &#x2013; "Called when a player falls and fall damage is calculated."
</div>
