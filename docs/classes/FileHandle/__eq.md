---
template: lua-class-function.html
title: __eq
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: FileHandle
  function: __eq
  realm: shared
  description: "Check if two file handles are equal."
  
  arguments:
  - position: 1
    name: "file"
    type: FileHandle
  - position: 2
    name: "other"
    type: FileHandle
  returns:
    - type: boolean
      description: "true if the file handles are equal, false otherwise."
---

<div class="lua__search__keywords">
FileHandle:__eq &#x2013; Check if two file handles are equal.
</div>
