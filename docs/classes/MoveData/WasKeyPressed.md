---
template: lua-class-function.html
title: WasKeyPressed
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: MoveData
  function: WasKeyPressed
  realm: shared
  description: "Checks if a key was pressed, where before it wasn't."
  
  arguments:
  - position: 1
    name: "moveData"
    type: MoveData
  - position: 2
    name: "buttonCode"
    type: number
  returns:
    - type: boolean
      description: "True if the key was pressed, false otherwise"
---

<div class="lua__search__keywords">
MoveData:WasKeyPressed &#x2013; Checks if a key was pressed, where before it wasn't.
</div>
