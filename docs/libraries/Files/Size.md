---
template: lua-library-function.html
title: Size
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Files
  function: Size
  realm: shared
  description: "Get the size of a file."
  
  arguments:
  - position: 1
    types:
      - name: "path"
        type: string
      - name: "file"
        type: FileHandle
  - position: 2
    name: "pathId"
    type: string
    default: "0"
  returns:
    - type: integer
      description: "The size of the file."
---

<div class="lua__search__keywords">
Files.Size &#x2013; Get the size of a file.
</div>
