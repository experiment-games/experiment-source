---
template: lua-library-function.html
title: RenameFile
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Files
  function: RenameFile
  realm: shared
  description: "Rename a file."
  
  arguments:
    - name: "oldPath"
      type: string
    - name: "newPath"
      type: string
    - name: "pathId"
      type: string
      default: "0"
  returns:
    - type: boolean
      description: "true if the file was renamed, false otherwise."
---

<div class="lua__search__keywords">
Files.RenameFile &#x2013; Rename a file.
</div>