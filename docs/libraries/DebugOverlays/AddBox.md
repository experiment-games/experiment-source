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
    - name: "start"
      type: Vector
    - name: "mins"
      type: Vector
    - name: "maxs"
      type: Vector
    - name: "angles"
      type: Angle
    - name: "duration"
      type: number
      default: "1"
    - name: "edgeColor"
      type: Color
      default: "faceColor"
  returns:
    
---

<div class="lua__search__keywords">
DebugOverlays.AddBox &#x2013; Add a box overlay to the screen with specific colors.
</div>
