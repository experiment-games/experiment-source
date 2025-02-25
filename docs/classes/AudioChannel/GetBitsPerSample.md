---
template: lua-class-function.html
title: GetBitsPerSample
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: AudioChannel
  function: GetBitsPerSample
  realm: shared
  description: "Gets the bits per sample of the audio channel (not properly implemented, always returns 0)"
  
  arguments:
  - position: 1
    name: "audioChannel"
    type: AudioChannel
  returns:
    - type: number
      description: "The bits per sample of the audio channel"
---

<div class="lua__search__keywords">
AudioChannel:GetBitsPerSample &#x2013; Gets the bits per sample of the audio channel (not properly implemented, always returns 0)
</div>
