---
template: lua-library-function.html
title: AddText
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: DebugOverlays
  function: AddText
  realm: shared
  description: "Add a text overlay to the screen with specific colors."
  
  arguments:
  - position: 1
    name: "origin"
    type: Vector
  - position: 2
    name: "text"
    type: string
  - position: 3
    name: "duration"
    type: number
    default: "1"
  - position: 4
    name: "viewCheck"
    type: boolean
    default: "false"
  - position: 5
    name: "lineOffset"
    type: number
    default: "64"
  - position: 6
    name: "color"
    type: Color
    default: "color_white"
  returns:
    
---

<div class="lua__search__keywords">
DebugOverlays.AddText &#x2013; Add a text overlay to the screen with specific colors.
</div>
