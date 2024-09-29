---
template: lua-library-function.html
title: AddTriangle
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: DebugOverlays
  function: AddTriangle
  realm: shared
  description: "Add a triangle overlay to the screen."
  
  arguments:
  - position: 1
    name: "p1"
    type: Vector
  - position: 2
    name: "p2"
    type: Vector
  - position: 3
    name: "p3"
    type: Vector
  - position: 4
    name: "duration"
    type: number
    default: "1"
  - position: 5
    name: "color"
    type: Color
    default: "color_white"
  - position: 6
    name: "noDepthTest"
    type: boolean
    default: "false"
  returns:
    
---

<div class="lua__search__keywords">
DebugOverlays.AddTriangle &#x2013; Add a triangle overlay to the screen.
</div>
