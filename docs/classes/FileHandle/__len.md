---
template: lua-class-function.html
title: __len
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: FileHandle
  function: __len
  realm: shared
  description: "Get the length of a file."
  
  arguments:
  - position: 1
    name: "file"
    type: FileHandle
  returns:
    - type: integer
      description: "The length of the file or nil if the file handle is invalid."
---

<div class="lua__search__keywords">
FileHandle:__len &#x2013; Get the length of a file.
</div>
