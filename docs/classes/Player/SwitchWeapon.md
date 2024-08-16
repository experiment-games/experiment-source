---
template: lua-class-function.html
title: SwitchWeapon
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Player
  function: SwitchWeapon
  realm: shared
  description: "Switch the player's weapon."
  
  arguments:
    - name: "weapon"
      type: Weapon
    - name: "subType"
      type: number
      default: "0"
  returns:
    - type: boolean
      description: "Whether the weapon was switched."
---

<div class="lua__search__keywords">
Player:SwitchWeapon &#x2013; Switch the player's weapon.
</div>
