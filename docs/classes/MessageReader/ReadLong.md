---
template: lua-class-function.html
title: ReadLong
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: MessageReader
  function: ReadLong
  realm: shared
  description: "Reads a long integer."
  
  arguments:
  - position: 1
    name: "reader"
    type: MessageReader
  - position: 2
    name: "numBits"
    type: number
  - position: 3
    name: "isSigned"
    type: boolean
    default: "true"
  returns:
    - type: integer
      description: "The long read."
---

<div class="lua__search__keywords">
MessageReader:ReadLong &#x2013; Reads a long integer.
</div>
