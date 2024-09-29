---
template: lua-class-function.html
title: EmitSound
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: EmitSound
  realm: shared
  description: "Emit sound."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "soundName"
    type: string
  - position: 3
    name: "soundLevel"
    type: enumeration/SOUND_LEVEL
    default: "SNDLVL_NORM"
  - position: 4
    name: "pitchPercent"
    type: number
    default: "100"
  - position: 5
    name: "volume"
    type: number
    default: "1"
  - position: 6
    name: "channel"
    type: number
    default: "pEntity->IsWeapon() ? CHAN_WEAPON : CHAN_AUTO"
  - position: 7
    name: "soundFlags"
    type: number
    default: "0"
  - position: 8
    name: "dsp"
    type: number
    default: "0"
  - position: 9
    name: "filter"
    type: RecipientFilter
    nillable: true
  returns:
    - type: number
      description: "The sound duration."
---

<div class="lua__search__keywords">
Entity:EmitSound &#x2013; Emit sound.
</div>
