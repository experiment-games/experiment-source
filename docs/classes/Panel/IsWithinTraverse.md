---
template: lua-class-function.html
title: IsWithinTraverse
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panel
  function: IsWithinTraverse
  realm: client
  description: "Checks if the panel is within the specified position"
  
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
    name: "traversePopups"
    type: boolean
  returns:
    - type: boolean
      description: "True if the panel is within the specified position, false otherwise"
---

<div class="lua__search__keywords">
Panel:IsWithinTraverse &#x2013; Checks if the panel is within the specified position
</div>
