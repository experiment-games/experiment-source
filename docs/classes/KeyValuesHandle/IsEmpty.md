---
template: lua-class-function.html
title: IsEmpty
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: KeyValuesHandle
  function: IsEmpty
  realm: shared
  description: "Check if the keyvalues is empty."
  
  arguments:
  - position: 1
    name: "keyValues"
    type: KeyValuesHandle
  - position: 2
    types:
      - name: "key"
        type: number
      - name: "key"
        type: string
    default: "0"
  returns:
    - type: boolean
      description: "true if the keyvalues is empty, false otherwise."
---

<div class="lua__search__keywords">
KeyValuesHandle:IsEmpty &#x2013; Check if the keyvalues is empty.
</div>
