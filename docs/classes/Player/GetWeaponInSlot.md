---
template: lua-class-function.html
title: GetWeaponInSlot
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Player
  function: GetWeaponInSlot
  realm: shared
  description: "Get the player's weapon in a slot."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "slot"
    type: number
  returns:
    - type: Entity
      description: "The player's weapon in the slot."
---

<div class="lua__search__keywords">
Player:GetWeaponInSlot &#x2013; Get the player's weapon in a slot.
</div>
