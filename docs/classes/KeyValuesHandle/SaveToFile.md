---
template: lua-class-function.html
title: SaveToFile
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: KeyValuesHandle
  function: SaveToFile
  realm: shared
  description: "Save keyvalues to a file."
  
  arguments:
    - name: "filename"
      type: string
    - name: "pathID"
      type: string
      default: "0"
  returns:
    - type: boolean
      description: "true if the keyvalues were saved, false otherwise."
---

<div class="lua__search__keywords">
KeyValuesHandle:SaveToFile &#x2013; Save keyvalues to a file.
</div>
