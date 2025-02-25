---
template: lua-class-function.html
title: WriteBitLong
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: MessageWriter
  function: WriteBitLong
  realm: shared
  description: "Writes an signed/unsigned number of the specified bits long."
  
  arguments:
  - position: 1
    name: "writer"
    type: MessageWriter
  - position: 2
    types:
      - name: "value"
        type: number
      - name: "value"
        type: number
  - position: 3
    types:
      - name: "numBits"
        type: number
      - name: "numBits"
        type: number
  - position: 4
    name: "isSigned"
    type: boolean
    default: "true"
  returns:
    
---

<div class="lua__search__keywords">
MessageWriter:WriteBitLong &#x2013; Writes an signed/unsigned number of the specified bits long.
</div>
