---
template: lua-class-function.html
title: RemoveAmmo
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Player
  function: RemoveAmmo
  realm: shared
  description: "Remove ammo from the player."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    types:
      - name: "amount"
        type: number
      - name: "amount"
        type: number
  - position: 3
    types:
      - name: "ammo type"
        type: number
      - name: "ammo name"
        type: string
  returns:
    
---

<div class="lua__search__keywords">
Player:RemoveAmmo &#x2013; Remove ammo from the player.
</div>
