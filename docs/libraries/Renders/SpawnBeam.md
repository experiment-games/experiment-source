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
      type: integer
    - name: "haloIndex"
      type: integer
    - name: "frameStart"
      type: integer
    - name: "frameRate"
      type: integer
    - name: "life"
      type: number
    - name: "width"
      type: integer
    - name: "endWidth"
      type: integer
    - name: "fadeLength"
      type: integer
    - name: "noise"
      type: integer
    - name: "colorAndBrightness"
      type: Color
    - name: "speed"
      type: integer
  returns:
    
---

<div class="lua__search__keywords">
Renders.SpawnBeam &#x2013; Spawns a temporary beam entity to render a beam. The alpha of the color servers as the brightness of it.
</div>
