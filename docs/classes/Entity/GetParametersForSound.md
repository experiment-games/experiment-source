---
template: lua-class-function.html
title: GetParametersForSound
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: GetParametersForSound
  realm: shared
  description: "Get parameters for sound."
  
  static: true
  arguments:
  - position: 1
    name: "sound"
    type: string
  - position: 2
    name: "actor"
    type: string
    default: "nullptr"
  returns:
    - type: boolean
      description: "True if parameters for sound exist, false otherwise."
    - type: soundparameters
      description: "The sound parameters."
---

<div class="lua__search__keywords">
_R.Entity.GetParametersForSound &#x2013; Get parameters for sound.
</div>
