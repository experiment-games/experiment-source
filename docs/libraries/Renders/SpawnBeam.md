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
  - position: 1
    name: "start"
    type: Vector
  - position: 2
    name: "end"
    type: Vector
  - position: 3
    name: "modelIndex"
    type: number
  - position: 4
    name: "haloIndex"
    type: number
  - position: 5
    name: "frameStart"
    type: number
  - position: 6
    name: "frameRate"
    type: number
  - position: 7
    name: "life"
    type: number
  - position: 8
    name: "width"
    type: number
  - position: 9
    name: "endWidth"
    type: number
  - position: 10
    name: "fadeLength"
    type: number
  - position: 11
    name: "noise"
    type: number
  - position: 12
    name: "colorAndBrightness"
    type: Color
  - position: 13
    name: "speed"
    type: number
  returns:
    
---

<div class="lua__search__keywords">
Renders.SpawnBeam &#x2013; Spawns a temporary beam entity to render a beam. The alpha of the color servers as the brightness of it.
</div>
