---
template: lua-library-function.html
title: RemoveSearchPath
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Files
  function: RemoveSearchPath
  realm: shared
  description: "Remove a search path."
  
  arguments:
    - name: "path"
      type: string
    - name: "pathId"
      type: string
      default: "0"
  returns:
    - type: boolean
      description: "true if the search path was removed, false otherwise."
---

<div class="lua__search__keywords">
Files.RemoveSearchPath &#x2013; Remove a search path.
</div>
