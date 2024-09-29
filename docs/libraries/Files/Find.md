---
template: lua-library-function.html
title: Find
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Files
  function: Find
  realm: shared
  description: "Find files."
  
  arguments:
  - position: 1
    name: "path"
    type: string
  - position: 2
    name: "pathId"
    type: string
    default: "\"GAME\""
  returns:
    - type: table
      description: "The files and directories."
---

<div class="lua__search__keywords">
Files.Find &#x2013; Find files.
</div>
