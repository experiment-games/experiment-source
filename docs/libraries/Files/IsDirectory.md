---
template: lua-library-function.html
title: IsDirectory
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Files
  function: IsDirectory
  realm: shared
  description: "Check if a path is a directory."
  
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
      description: "true if the path is a directory, false otherwise."
---

<div class="lua__search__keywords">
Files.IsDirectory &#x2013; Check if a path is a directory.
</div>
