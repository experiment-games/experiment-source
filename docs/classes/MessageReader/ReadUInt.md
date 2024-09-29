---
template: lua-class-function.html
title: ReadUInt
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: MessageReader
  function: ReadUInt
  realm: shared
  description: "Reads an unsigned integer"
  
  arguments:
  - position: 1
    name: "reader"
    type: MessageReader
  - position: 2
    name: "numBits"
    type: number
  returns:
    - type: integer
      description: "The integer read."
---

<div class="lua__search__keywords">
MessageReader:ReadUInt &#x2013; Reads an unsigned integer
</div>
