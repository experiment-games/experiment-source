---
template: lua-library-function.html
title: CopyFile
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Engines
  function: CopyFile
  realm: server
  description: "Copy a file."
  
  arguments:
  - position: 1
    name: "source"
    type: string
  - position: 2
    name: "destination"
    type: string
  returns:
    - type: boolean
      description: "True if the file was copied, false otherwise."
---

<div class="lua__search__keywords">
Engines.CopyFile &#x2013; Copy a file.
</div>
