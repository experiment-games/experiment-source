---
template: lua-library-function.html
title: AddBox
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: DebugOverlays
  function: AddBox
  realm: shared
  description: "Add a box overlay to the screen with specific colors."
  
  arguments:
  - position: 1
    name: "start"
    type: Vector
  - position: 2
    name: "mins"
    type: Vector
  - position: 3
    name: "maxs"
    type: Vector
  - position: 4
    name: "angles"
    type: Angle
  - position: 5
    name: "duration"
    type: number
    default: "1"
  - position: 6
    name: "faceColor"
    type: Color
    default: "color_white"
  - position: 7
    name: "edgeColor"
    type: Color
    default: "faceColor"
  returns:
    
---

<div class="lua__search__keywords">
DebugOverlays.AddBox &#x2013; Add a box overlay to the screen with specific colors.
</div>
