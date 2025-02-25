---
template: lua-class-function.html
title: ActivityOverride
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Weapon
  function: ActivityOverride
  realm: shared
  description: "Asks which Activity override the weapon wants."
  
  arguments:
  - position: 1
    name: "entity"
    type: Weapon
  - position: 2
    name: "activity"
    type: number
  returns:
    - type: enumeration/ACTIVITY
      description: "Override activity ID."
    - type: boolean
      description: "Whether the activity is required."
---

<div class="lua__search__keywords">
Weapon:ActivityOverride &#x2013; Asks which Activity override the weapon wants.
</div>
