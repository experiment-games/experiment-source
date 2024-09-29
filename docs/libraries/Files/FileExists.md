---
template: lua-library-function.html
title: FileExists
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Files
  function: FileExists
  realm: shared
  description: "Check if the file or directory exists."
  
  arguments:
  - position: 1
    name: "path"
    type: string
  - position: 2
    name: "pathId"
    type: string
    default: "0"
  returns:
    - type: boolean
      description: "true if the file or directory exists, false otherwise."
---

<div class="lua__search__keywords">
Files.FileExists &#x2013; Check if the file or directory exists.
</div>
