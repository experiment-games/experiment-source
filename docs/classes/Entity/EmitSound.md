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
    - name: "soundName"
      type: string
    - name: "soundLevel"
      type: number
      default: "75"
    - name: "pitchPercent"
      type: number
      default: "100"
    - name: "volume"
      type: number
      default: "1"
    - name: "channel"
      type: number
      default: "pEntity->IsWeapon() ? CHAN_WEAPON : CHAN_AUTO"
    - name: "soundFlags"
      type: number
      default: "0"
    - name: "dsp"
      type: number
      default: "0"
    - name: "filter"
      type: RecipientFilter
  returns:
    - type: number
      description: "The sound duration."
---

<div class="lua__search__keywords">
Entity:EmitSound &#x2013; Emit sound.
</div>
