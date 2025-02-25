---
template: lua-library-function.html
title: Play
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Sounds
  function: Play
  realm: shared
  description: "Plays a sound emitting from a place in the world. Not properly tested for sound script names (didn't work when I tried it)"
  
  arguments:
  - position: 1
    name: "soundName"
    type: string
  - position: 2
    name: "origin"
    type: Vector
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
    name: "dsp"
    type: number
    default: "0"
  returns:
    
---

<div class="lua__search__keywords">
Sounds.Play &#x2013; Plays a sound emitting from a place in the world. Not properly tested for sound script names (didn't work when I tried it)
</div>
