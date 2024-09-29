---
template: lua-class-function.html
title: ReadBytes
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: MessageReader
  function: ReadBytes
  realm: shared
  description: "Reads a number of bytes."
  
  arguments:
  - position: 1
    name: "reader"
    type: MessageReader
  - position: 2
    name: "length"
    type: number
  returns:
    - type: string
      description: "The bytes read."
---

<div class="lua__search__keywords">
MessageReader:ReadBytes &#x2013; Reads a number of bytes.
</div>
