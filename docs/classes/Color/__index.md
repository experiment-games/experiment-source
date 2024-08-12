---
template: lua-class-function.html
title: __index
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Color
  function: __index
  realm: shared
  description: "Metamethod for when the index field doesn't exist. Returns values when indexing r, g, b, a or 1, 2, 3, 4."
  
  arguments:
    - name: "field"
      type: string
  returns:
    - type: any
      description: "The value of the field."
---

<div class="lua__search__keywords">
Color:__index &#x2013; Metamethod for when the index field doesn't exist. Returns values when indexing r, g, b, a or 1, 2, 3, 4.
</div>
