---
template: lua-class-function.html
title: GetSoundEmissionOrigin
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: GetSoundEmissionOrigin
  realm: server
  description: "From a source SDK comment 'Used by the PAS filters to ask the entity where in world space the sounds it emits come from. This is used right now because if you have something sitting on an incline, using our axis-aligned bounding boxes can return a position in solid space, so you won't hear sounds emitted by the object. For now, we're hacking around it by moving the sound emission origin up on certain objects like vehicles..'"
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  returns:
    - type: Vector
      description: "The sound emission origin"
---

<div class="lua__search__keywords">
Entity:GetSoundEmissionOrigin &#x2013; From a source SDK comment 'Used by the PAS filters to ask the entity where in world space the sounds it emits come from. This is used right now because if you have something sitting on an incline, using our axis-aligned bounding boxes can return a position in solid space, so you won't hear sounds emitted by the object. For now, we're hacking around it by moving the sound emission origin up on certain objects like vehicles..'
</div>
