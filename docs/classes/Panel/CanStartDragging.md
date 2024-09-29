---
template: lua-class-function.html
title: CanStartDragging
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panel
  function: CanStartDragging
  realm: client
  description: "Checks if dragging can start from the panel"
  
  arguments:
  - position: 1
    name: "panel"
    type: Panel
  - position: 2
    name: "startX"
    type: number
  - position: 3
    name: "startY"
    type: number
  - position: 4
    name: "cursorX"
    type: number
  - position: 5
    name: "cursorY"
    type: number
  returns:
    - type: boolean
      description: "Whether dragging can start"
---

<div class="lua__search__keywords">
Panel:CanStartDragging &#x2013; Checks if dragging can start from the panel
</div>
