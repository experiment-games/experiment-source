---
template: lua-class-function.html
title: Get3dCone
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: AudioChannel
  function: Get3dCone
  realm: shared
  description: "Gets the 3D cone of the audio channel"
  
  arguments:
  - position: 1
    name: "audioChannel"
    type: AudioChannel
  returns:
    - type: number
      description: "The inner angle of the 3D cone"
    - type: number
      description: "The outer angle of the 3D cone"
    - type: number
      description: "The outer volume of the 3D cone"
---

<div class="lua__search__keywords">
AudioChannel:Get3dCone &#x2013; Gets the 3D cone of the audio channel
</div>
