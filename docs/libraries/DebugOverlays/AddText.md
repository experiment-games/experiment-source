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
    - name: "origin"
      type: Vector
    - name: "text"
      type: string
    - name: "duration"
      type: number
      default: "1"
    - name: "viewCheck"
      type: boolean
      default: "false"
    - name: "lineOffset"
      type: number
      default: "64"
  returns:
    
---

<div class="lua__search__keywords">
DebugOverlays.AddText &#x2013; Add a text overlay to the screen with specific colors.
</div>
