---
template: lua-library-function.html
title: DrawSetColor
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Surfaces
  function: DrawSetColor
  realm: client
  description: "Sets the color"
  
  arguments:
  - position: 1
    types:
      - name: "red"
        type: number
      - name: "color"
        type: Color
  - position: 2
    name: "green"
    type: number
  - position: 3
    name: "blue"
    type: number
  - position: 4
    name: "alpha"
    type: number
    default: "255"
  returns:
    
---

<div class="lua__search__keywords">
Surfaces.DrawSetColor &#x2013; Sets the color
</div>
