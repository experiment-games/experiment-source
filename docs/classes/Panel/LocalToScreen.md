---
template: lua-class-function.html
title: LocalToScreen
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panel
  function: LocalToScreen
  realm: client
  description: "Converts a local position to a screen position"
  
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
  returns:
    - type: integer
      description: "The x position of the screen"
    - type: integer
      description: "The y position of the screen"
---

<div class="lua__search__keywords">
Panel:LocalToScreen &#x2013; Converts a local position to a screen position
</div>
