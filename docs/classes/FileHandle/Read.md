---
template: lua-class-function.html
title: Read
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: FileHandle
  function: Read
  realm: shared
  description: "Read from a file."
  
  arguments:
    - name: "size"
      type: number
      default: "GET_FILE_SIZE"
  returns:
    - type: integer
      description: "The number of bytes read."
    - type: string
      description: "The data read."
---

<div class="lua__search__keywords">
FileHandle:Read &#x2013; Read from a file.
</div>
