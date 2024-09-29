---
template: lua-library-function.html
title: ShakeScreen
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Utilities
  function: ShakeScreen
  realm: shared
  description: "Apply screen shake effect"
  
  arguments:
  - position: 1
    name: "center"
    type: Vector
  - position: 2
    name: "amplitude"
    type: number
  - position: 3
    name: "frequency"
    type: number
  - position: 4
    name: "duration"
    type: number
  - position: 5
    name: "radius"
    type: number
  - position: 6
    name: "command"
    type: enumeration/SHAKE_COMMAND
  - position: 7
    name: "localAmplitude"
    type: boolean
    default: "false"
  returns:
    
---

<div class="lua__search__keywords">
Utilities.ShakeScreen &#x2013; Apply screen shake effect
</div>
