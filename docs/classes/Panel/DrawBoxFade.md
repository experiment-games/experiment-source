---
template: lua-class-function.html
title: DrawBoxFade
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panel
  function: DrawBoxFade
  realm: client
  description: "Draws a fading box on the panel"
  
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
    name: "alpha0"
    type: number
  - position: 9
    name: "alpha1"
    type: number
  - position: 10
    name: "isHorizontal"
    type: boolean
  - position: 11
    name: "isHollow"
    type: boolean
    default: "false"
  returns:
    
---

<div class="lua__search__keywords">
Panel:DrawBoxFade &#x2013; Draws a fading box on the panel
</div>
