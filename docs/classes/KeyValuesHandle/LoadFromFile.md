---
template: lua-class-function.html
title: LoadFromFile
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: KeyValuesHandle
  function: LoadFromFile
  realm: shared
  description: "Load keyvalues from a file."
  
  arguments:
    - name: "filename"
      type: string
    - name: "pathID"
      type: string
      default: "0"
  returns:
    - type: boolean
      description: "true if the keyvalues were loaded, false otherwise."
---

<div class="lua__search__keywords">
KeyValuesHandle:LoadFromFile &#x2013; Load keyvalues from a file.
</div>
