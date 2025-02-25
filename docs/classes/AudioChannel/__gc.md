---
template: lua-class-function.html
title: __gc
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: AudioChannel
  function: __gc
  realm: shared
  description: "Called when the audio channel is garbage collected. Cleans up the audio channel so it can't be reused and memory is freed."
  
  arguments:
  - position: 1
    name: "audioChannel"
    type: AudioChannel
  returns:
    
---

<div class="lua__search__keywords">
AudioChannel:__gc &#x2013; Called when the audio channel is garbage collected. Cleans up the audio channel so it can't be reused and memory is freed.
</div>
