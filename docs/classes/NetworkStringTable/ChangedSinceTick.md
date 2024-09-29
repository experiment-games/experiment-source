---
template: lua-class-function.html
title: ChangedSinceTick
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: NetworkStringTable
  function: ChangedSinceTick
  realm: shared
  description: "Check if the table has changed since the given tick."
  
  arguments:
  - position: 1
    name: "networkStringTable"
    type: NetworkStringTable
  - position: 2
    name: "tick"
    type: number
  returns:
    - type: boolean
      description: "True if the table has changed, false otherwise."
---

<div class="lua__search__keywords">
NetworkStringTable:ChangedSinceTick &#x2013; Check if the table has changed since the given tick.
</div>
