---
template: lua-class-function.html
title: GetWeapon
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Player
  function: GetWeapon
  realm: shared
  description: "Get the player's weapon."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "className"
    type: string
  returns:
    - type: Entity
      description: "The player's weapon."
---

<div class="lua__search__keywords">
Player:GetWeapon &#x2013; Get the player's weapon.
</div>
