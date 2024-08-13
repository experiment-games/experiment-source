---
template: lua-class-function.html
title: FindKey
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: KeyValuesHandle
  function: FindKey
  realm: shared
  description: "Find a key in the keyvalues."
  
  argumentSets:
    - arguments:
      - name: "key"
        type: number
    - arguments:
      - name: "key"
        type: string
      - name: "recursive"
        type: boolean
        default: "false"
  returns:
    - type: KeyValuesHandle
      description: "The keyvalues key."
---

<div class="lua__search__keywords">
KeyValuesHandle:FindKey &#x2013; Find a key in the keyvalues.
</div>
