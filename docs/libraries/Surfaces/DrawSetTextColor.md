---
template: lua-library-function.html
title: DrawSetTextColor
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Surfaces
  function: DrawSetTextColor
  realm: client
  description: "Sets the text color"
  
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
Surfaces.DrawSetTextColor &#x2013; Sets the text color
</div>
