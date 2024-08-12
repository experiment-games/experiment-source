---
template: lua-library-function.html
title: AddSearchPath
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Files
  function: AddSearchPath
  realm: shared
  description: "Add a search path to the filesystem."
  
  arguments:
    - name: "path"
      type: string
    - name: "pathId"
      type: string
    - name: "addType"
      type: number
      default: "PATH_ADD_TO_TAIL"
  returns:
    
---

<div class="lua__search__keywords">
Files.AddSearchPath &#x2013; Add a search path to the filesystem.
</div>
