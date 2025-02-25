---
template: lua-class-function.html
title: GetFft
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: AudioChannel
  function: GetFft
  realm: shared
  description: "Gets the Discrete Fourier Transform of the audio channel"
  
  arguments:
  - position: 1
    name: "audioChannel"
    type: AudioChannel
  - position: 2
    name: "size"
    type: integer
    default: "2048"
  returns:
    - type: table
      description: "The Discrete Fourier Transform of the audio channel"
---

<div class="lua__search__keywords">
AudioChannel:GetFft &#x2013; Gets the Discrete Fourier Transform of the audio channel
</div>
