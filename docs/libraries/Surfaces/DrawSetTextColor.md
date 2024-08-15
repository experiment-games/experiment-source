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
  
  argumentSets:
    - arguments:
      - name: "red"
        type: number
      - name: "green"
        type: number
      - name: "blue"
        type: number
      - name: "alpha"
        type: number
        default: "255"
    - arguments:
      - name: "color"
        type: Color
  returns:
    
---

<div class="lua__search__keywords">
Surfaces.DrawSetTextColor &#x2013; Sets the text color
</div>
