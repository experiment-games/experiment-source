---
template: lua-class-function.html
title: WasKeyDown
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: MoveData
  function: WasKeyDown
  realm: shared
  description: "Checks if a key was down before"
  
  arguments:
  - position: 1
    name: "moveData"
    type: MoveData
  - position: 2
    name: "buttonCode"
    type: number
  returns:
    - type: boolean
      description: "True if the key was down, false otherwise"
---

<div class="lua__search__keywords">
MoveData:WasKeyDown &#x2013; Checks if a key was down before
</div>
