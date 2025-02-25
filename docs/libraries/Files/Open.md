---
template: lua-library-function.html
title: Open
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Files
  function: Open
  realm: shared
  description: "Open a file."
  
  arguments:
  - position: 1
    name: "filePath"
    type: string
  - position: 2
    name: "readMode"
    type: string
  - position: 3
    name: "pathId"
    type: string
    default: "\"DATA\""
  returns:
    - type: FileHandle
      description: "The file handle."
---

<div class="lua__search__keywords">
Files.Open &#x2013; Open a file.
</div>
