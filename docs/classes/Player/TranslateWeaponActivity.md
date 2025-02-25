---
template: lua-class-function.html
title: TranslateWeaponActivity
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Player
  function: TranslateWeaponActivity
  realm: shared
  description: "Translate a weapon activity."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "activity"
    type: enumeration/ACTIVITY
  returns:
    - type: enumeration/ACTIVITY
      description: "The translated activity."
---

<div class="lua__search__keywords">
Player:TranslateWeaponActivity &#x2013; Translate a weapon activity.
</div>
