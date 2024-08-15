---
template: lua-library-function.html
title: SpawnBeam
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Renders
  function: SpawnBeam
  realm: server
  description: "Spawns a temporary beam entity to render a beam. The alpha of the color servers as the brightness of it."
  
  arguments:
    - name: "start"
      type: Vector
    - name: "end"
      type: Vector
    - name: "modelIndex"
      type: number
    - name: "haloIndex"
      type: number
    - name: "frameStart"
      type: number
    - name: "frameRate"
      type: number
    - name: "life"
      type: number
    - name: "width"
      type: number
    - name: "endWidth"
      type: number
    - name: "fadeLength"
      type: number
    - name: "noise"
      type: number
    - name: "colorAndBrightness"
      type: Color
    - name: "speed"
      type: number
  returns:
    
---

<div class="lua__search__keywords">
Renders.SpawnBeam &#x2013; Spawns a temporary beam entity to render a beam. The alpha of the color servers as the brightness of it.
</div>
