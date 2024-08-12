---
template: lua-library-function.html
title: Read
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Files
  function: Read
  realm: shared
  description: "Read from a file."
  
  arguments:
    - name: "file"
      type: FileHandle
    - name: "size"
      type: number
  returns:
    - type: integer
      description: "The number of bytes read."
    - type: string
      description: "The data read."
---

<div class="lua__search__keywords">
Files.Read &#x2013; Read from a file.
</div>
