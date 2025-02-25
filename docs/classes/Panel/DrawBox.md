---
template: lua-class-function.html
title: DrawBox
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panel
  function: DrawBox
  realm: client
  description: "Draws a box on the panel"
  
  arguments:
  - position: 1
    name: "panel"
    type: Panel
  - position: 2
    name: "x"
    type: number
  - position: 3
    name: "y"
    type: number
  - position: 4
    name: "wide"
    type: number
  - position: 5
    name: "tall"
    type: number
  - position: 6
    name: "color"
    type: Color
  - position: 7
    name: "normalizedAlpha"
    type: number
  - position: 8
    name: "isHollow"
    type: boolean
    default: "false"
  returns:
    
---

<div class="lua__search__keywords">
Panel:DrawBox &#x2013; Draws a box on the panel
</div>
