---
template: lua-library-function.html
title: Write
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Files
  function: Write
  realm: shared
  description: "Write to a file."
  
  arguments:
  - position: 1
    name: "input"
    type: string
  - position: 2
    name: "file"
    type: FileHandle
  returns:
    - type: integer
      description: "The number of bytes written."
---

<div class="lua__search__keywords">
Files.Write &#x2013; Write to a file.
</div>
