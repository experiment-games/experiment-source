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
  
  argumentSets:
    - arguments:
      - name: "ammoCount"
        type: number
      - name: "ammoType"
        type: number
    - arguments:
      - name: "ammoName"
        type: string
  returns:
    - type: integer
      description: "The amount of ammo actually given."
---

<div class="lua__search__keywords">
Player:GiveAmmo &#x2013; Give ammo to the player.
</div>
