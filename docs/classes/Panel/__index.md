---
template: lua-class-function.html
title: __index
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panel
  function: __index
  realm: client
  description: "Metamethod called when a non-existent field is indexed"
  
  arguments:
  - position: 1
    name: "panel"
    type: Panel
  - position: 2
    name: "field"
    type: string
  returns:
    - type: any
      description: "The value of the field"
---

<div class="lua__search__keywords">
Panel:__index &#x2013; Metamethod called when a non-existent field is indexed
</div>
