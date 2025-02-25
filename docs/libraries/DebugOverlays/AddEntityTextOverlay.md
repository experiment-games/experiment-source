---
template: lua-library-function.html
title: AddEntityTextOverlay
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: DebugOverlays
  function: AddEntityTextOverlay
  realm: shared
  description: "Displays 2D text at the specified coordinates."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "lineOffset"
    type: number
  - position: 3
    name: "text"
    type: string
  - position: 4
    name: "duration"
    type: number
    default: "1"
  - position: 5
    name: "color"
    type: Color
    default: "color_white"
  returns:
    
---

<div class="lua__search__keywords">
DebugOverlays.AddEntityTextOverlay &#x2013; Displays 2D text at the specified coordinates.
</div>
