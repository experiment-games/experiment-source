---
template: lua-library-function.html
title: SetFileWritable
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Files
  function: SetFileWritable
  realm: shared
  description: "Set a file as writable."
  
  arguments:
    - name: "path"
      type: string
    - name: "writable"
      type: boolean
    - name: "pathId"
      type: string
      default: "0"
  returns:
    - type: boolean
      description: "true if the file was set as writable, false otherwise."
---

<div class="lua__search__keywords">
Files.SetFileWritable &#x2013; Set a file as writable.
</div>
