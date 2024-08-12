---
template: lua-library-function.html
title: IsFileWritable
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Files
  function: IsFileWritable
  realm: shared
  description: "Check if a file is writable."
  
  arguments:
    - name: "path"
      type: string
    - name: "pathId"
      type: string
      default: "0"
  returns:
    - type: boolean
      description: "true if the file is writable, false otherwise."
---

<div class="lua__search__keywords">
Files.IsFileWritable &#x2013; Check if a file is writable.
</div>
