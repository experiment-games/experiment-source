---
template: lua-library-function.html
title: AddScreenText
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: DebugOverlays
  function: AddScreenText
  realm: shared
  description: "Add a screen text overlay."
  
  arguments:
  - position: 1
    name: "x"
    type: number
  - position: 2
    name: "y"
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
DebugOverlays.AddScreenText &#x2013; Add a screen text overlay.
</div>
