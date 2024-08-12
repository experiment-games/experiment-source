---
template: lua-library-function.html
title: Size
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Files
  function: Size
  realm: shared
  description: "Get the size of a file."
  
  argumentSets:
    - arguments:
      - name: "path"
        type: string
    - arguments:
      - name: "file"
        type: FileHandle
  returns:
    - type: integer
      description: "The size of the file."
---

<div class="lua__search__keywords">
Files.Size &#x2013; Get the size of a file.
</div>
