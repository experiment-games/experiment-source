---
template: lua-class-function.html
title: DefaultReload
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Weapon
  function: DefaultReload
  realm: shared
  description: "Default reload."
  
  arguments:
  - position: 1
    name: "entity"
    type: Weapon
  - position: 2
    name: "clipSize1"
    type: number
  - position: 3
    name: "clipSize2"
    type: number
  - position: 4
    name: "activity"
    type: number
  returns:
    - type: boolean
      description: "Whether the weapon was successfully reloaded."
---

<div class="lua__search__keywords">
Weapon:DefaultReload &#x2013; Default reload.
</div>
