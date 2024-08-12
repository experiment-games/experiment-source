---
template: lua-library-function.html
title: AddPackFile
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Files
  function: AddPackFile
  realm: shared
  description: "Add a pack file to the search path."
  
  arguments:
    - name: "path"
      type: string
    - name: "pathId"
      type: string
  returns:
    - type: boolean
      description: "true if the pack file was added, false otherwise."
---

<div class="lua__search__keywords">
Files.AddPackFile &#x2013; Add a pack file to the search path.
</div>
