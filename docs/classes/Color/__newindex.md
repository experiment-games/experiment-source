---
template: lua-class-function.html
title: __newindex
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Color
  function: __newindex
  realm: shared
  description: "Metamethod for when the newindex field doesn't exist. Sets values when indexing r, g, b, a or 1, 2, 3, 4."
  
  arguments:
  - position: 1
    name: "color"
    type: Color
  - position: 2
    name: "field"
    type: string
  returns:
    
---

<div class="lua__search__keywords">
Color:__newindex &#x2013; Metamethod for when the newindex field doesn't exist. Sets values when indexing r, g, b, a or 1, 2, 3, 4.
</div>
