---
template: lua-class-function.html
title: SetTime
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: AudioChannel
  function: SetTime
  realm: shared
  description: "Sets the time of the audio channel"
  
  arguments:
  - position: 1
    name: "audioChannel"
    type: AudioChannel
  - position: 2
    name: "time"
    type: number
  - position: 3
    name: "dontDecode"
    type: boolean
    default: "false"
  returns:
    
---

<div class="lua__search__keywords">
AudioChannel:SetTime &#x2013; Sets the time of the audio channel
</div>
