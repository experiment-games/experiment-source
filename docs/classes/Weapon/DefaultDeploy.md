---
template: lua-class-function.html
title: DefaultDeploy
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Weapon
  function: DefaultDeploy
  realm: shared
  description: "Default deploy."
  
  arguments:
  - position: 1
    name: "entity"
    type: Weapon
  - position: 2
    name: "viewModel"
    type: string
  - position: 3
    name: "worldModel"
    type: string
  - position: 4
    name: "activity"
    type: number
  - position: 5
    name: "animationExtension"
    type: string
  returns:
    - type: boolean
      description: "Whether the weapon was successfully deployed."
---

<div class="lua__search__keywords">
Weapon:DefaultDeploy &#x2013; Default deploy.
</div>
