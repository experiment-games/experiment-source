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
  - position: 3
    name: "recursive"
    type: boolean
    default: "false"
  returns:
    - type: KeyValuesHandle
      description: "The keyvalues key."
---

<div class="lua__search__keywords">
KeyValuesHandle:FindKey &#x2013; Find a key in the keyvalues.
</div>
