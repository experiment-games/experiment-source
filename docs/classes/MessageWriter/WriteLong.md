---
template: lua-class-function.html
title: WriteLong
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: MessageWriter
  function: WriteLong
  realm: shared
  description: "Writes a long integer."
  
  arguments:
  - position: 1
    name: "writer"
    type: MessageWriter
  - position: 2
    name: "long"
    type: number
  - position: 3
    name: "numBits"
    type: number
  - position: 4
    name: "isSigned"
    type: boolean
    default: "true"
  returns:
    
---

<div class="lua__search__keywords">
MessageWriter:WriteLong &#x2013; Writes a long integer.
</div>
