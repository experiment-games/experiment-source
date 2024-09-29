---
template: lua-class-function.html
title: ReadBitLong
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: MessageReader
  function: ReadBitLong
  realm: shared
  description: "Reads an signed/unsigned number of the specified bits long."
  
  arguments:
  - position: 1
    name: "reader"
    type: MessageReader
  - position: 2
    types:
      - name: "numBits"
        type: number
      - name: "numBits"
        type: number
  - position: 3
    name: "isSigned"
    type: boolean
    default: "true"
  returns:
    - type: integer
      description: "The number read."
---

<div class="lua__search__keywords">
MessageReader:ReadBitLong &#x2013; Reads an signed/unsigned number of the specified bits long.
</div>
