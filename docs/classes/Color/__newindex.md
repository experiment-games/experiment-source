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
  
  argumentSets:
    - arguments:
      - name: "field"
        type: string
      - name: "red"
        type: number
    - arguments:
      - name: "green"
        type: number
    - arguments:
      - name: "blue"
        type: number
    - arguments:
      - name: "alpha"
        type: number
  returns:
    
---

<div class="lua__search__keywords">
Color:__newindex &#x2013; Metamethod for when the newindex field doesn't exist. Sets values when indexing r, g, b, a or 1, 2, 3, 4.
</div>
