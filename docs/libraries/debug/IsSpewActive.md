---
template: lua-library-function.html
title: IsSpewActive
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: debug
  function: IsSpewActive
  realm: shared
  description: "Check if spew output is active."
  
  arguments:
  - position: 1
    name: "groupName"
    type: string
  - position: 2
    name: "level"
    type: number
  returns:
    - type: boolean
      description: "True if spew output is active, false otherwise."
---

<div class="lua__search__keywords">
debug.IsSpewActive &#x2013; Check if spew output is active.
</div>
