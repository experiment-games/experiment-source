---
template: lua-class-function.html
title: FrameAdvance
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: FrameAdvance
  realm: client
  description: "Advance the frame."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "interval"
    type: number
    default: "0.0f"
  returns:
    - type: number
      description: "The new cycle"
---

<div class="lua__search__keywords">
Entity:FrameAdvance &#x2013; Advance the frame.
</div>
