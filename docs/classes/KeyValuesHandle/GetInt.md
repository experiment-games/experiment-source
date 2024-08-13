---
template: lua-class-function.html
title: GetInt
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: KeyValuesHandle
  function: GetInt
  realm: shared
  description: "Get an integer from the keyvalues."
  
  argumentSets:
    - arguments:
      - name: "key"
        type: number
      - name: "default"
        type: number
        default: "0"
    - arguments:
      - name: "key"
        type: string
        default: "0"
      - name: "default"
        type: number
        default: "0"
  returns:
    - type: integer
      description: "The value as an integer."
---

<div class="lua__search__keywords">
KeyValuesHandle:GetInt &#x2013; Get an integer from the keyvalues.
</div>
