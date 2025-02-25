---
template: lua-class-function.html
title: GiveAmmo
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Player
  function: GiveAmmo
  realm: server
  description: "Give ammo to the player."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "ammoCount"
    type: number
  - position: 3
    types:
      - name: "ammoType"
        type: number
      - name: "ammoName"
        type: string
  - position: 4
    types:
      - name: "shouldSuppressSound"
        type: boolean
    default: "false"
      - name: "shouldSuppressSound"
        type: boolean
    default: "false"
  returns:
    - type: integer
      description: "The amount of ammo actually given."
---

<div class="lua__search__keywords">
Player:GiveAmmo &#x2013; Give ammo to the player.
</div>
