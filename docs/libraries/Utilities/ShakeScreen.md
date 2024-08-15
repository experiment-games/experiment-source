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
    - name: "center"
      type: Vector
    - name: "amplitude"
      type: number
    - name: "frequency"
      type: number
    - name: "duration"
      type: number
    - name: "radius"
      type: number
    - name: "command"
      type: enumeration/SHAKE_COMMAND
    - name: "localAmplitude"
      type: boolean
      default: "false"
  returns:
    
---

<div class="lua__search__keywords">
Utilities.ShakeScreen &#x2013; Apply screen shake effect
</div>
