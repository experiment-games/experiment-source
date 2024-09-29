---
template: lua-library-function.html
title: AddLine
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: DebugOverlays
  function: AddLine
  realm: shared
  description: "Add a line overlay to the screen with alpha."
  
  arguments:
  - position: 1
    name: "start"
    type: Vector
  - position: 2
    name: "end"
    type: Vector
  - position: 3
    name: "duration"
    type: number
    default: "1"
  - position: 4
    name: "color"
    type: Color
    default: "color_white"
  - position: 5
    name: "noDepthTest"
    type: boolean
    default: "false"
  returns:
    
---

<div class="lua__search__keywords">
DebugOverlays.AddLine &#x2013; Add a line overlay to the screen with alpha.
</div>
