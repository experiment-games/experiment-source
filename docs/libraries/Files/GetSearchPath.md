---
template: lua-library-function.html
title: GetSearchPath
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Files
  function: GetSearchPath
  realm: shared
  description: "Get the search path."
  
  arguments:
  - position: 1
    name: "path"
    type: string
  - position: 2
    name: "bGetPackFiles"
    type: boolean
    default: "false"
  returns:
    - type: string
      description: "The search path."
---

<div class="lua__search__keywords">
Files.GetSearchPath &#x2013; Get the search path.
</div>
