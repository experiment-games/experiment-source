---
template: lua-library-function.html
title: Precache
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Files
  function: Precache
  realm: shared
  description: "Precache a file."
  
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
      description: "true if the file was precached, false otherwise."
---

<div class="lua__search__keywords">
Files.Precache &#x2013; Precache a file.
</div>
