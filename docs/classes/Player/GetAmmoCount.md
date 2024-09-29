---
template: lua-class-function.html
title: GetAmmoCount
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Player
  function: GetAmmoCount
  realm: shared
  description: "Get the player's ammo count."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    types:
      - name: "index"
        type: number
      - name: "name"
        type: string
  returns:
    - type: integer
      description: "The player's ammo count."
---

<div class="lua__search__keywords">
Player:GetAmmoCount &#x2013; Get the player's ammo count.
</div>
