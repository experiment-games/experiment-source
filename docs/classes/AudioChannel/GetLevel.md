---
template: lua-class-function.html
title: GetLevel
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: AudioChannel
  function: GetLevel
  realm: shared
  description: "Gets the level of the audio channel"
  
  arguments:
  - position: 1
    name: "audioChannel"
    type: AudioChannel
  returns:
    - type: number
      description: "The left level of the audio channel"
    - type: number
      description: "The right level of the audio channel"
---

<div class="lua__search__keywords">
AudioChannel:GetLevel &#x2013; Gets the level of the audio channel
</div>
