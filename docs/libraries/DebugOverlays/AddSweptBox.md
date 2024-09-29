---
template: lua-library-function.html
title: AddSweptBox
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: DebugOverlays
  function: AddSweptBox
  realm: shared
  description: "Add a swept box overlay to the screen."
  
  arguments:
  - position: 1
    name: "start"
    type: Vector
  - position: 2
    name: "end"
    type: Vector
  - position: 3
    name: "mins"
    type: Vector
  - position: 4
    name: "maxs"
    type: Vector
  - position: 5
    name: "angles"
    type: Angle
  - position: 6
    name: "duration"
    type: number
    default: "1"
  - position: 7
    name: "color"
    type: Color
    default: "color_white"
  returns:
    
---

<div class="lua__search__keywords">
DebugOverlays.AddSweptBox &#x2013; Add a swept box overlay to the screen.
</div>
