---
template: lua-class-function.html
title: ReadData
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: MessageReader
  function: ReadData
  realm: shared
  description: "Reads binary data, represented in Lua as a string"
  
  arguments:
  - position: 1
    name: "reader"
    type: MessageReader
  - position: 2
    name: "length"
    type: number
  returns:
    - type: string
      description: "The data read."
---

<div class="lua__search__keywords">
MessageReader:ReadData &#x2013; Reads binary data, represented in Lua as a string
</div>
