---
template: lua-library-function.html
title: UnzipFile
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Files
  function: UnzipFile
  realm: shared
  description: "Unzip a file."
  
  arguments:
  - position: 1
    name: "source"
    type: string
  - position: 2
    name: "destination"
    type: string
  - position: 3
    name: "pathId"
    type: string
  returns:
    - type: boolean
      description: "true if the file was unzipped, false otherwise."
---

<div class="lua__search__keywords">
Files.UnzipFile &#x2013; Unzip a file.
</div>
