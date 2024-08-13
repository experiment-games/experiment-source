---
template: lua-class-function.html
title: GetString
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: KeyValuesHandle
  function: GetString
  realm: shared
  description: "Get a string from the keyvalues."
  
  argumentSets:
    - arguments:
      - name: "key"
        type: number
      - name: "default"
        type: string
        default: "\"\""
    - arguments:
      - name: "key"
        type: string
        default: "0"
      - name: "default"
        type: string
        default: "\"\""
  returns:
    - type: string
      description: "The value as a string."
---

<div class="lua__search__keywords">
KeyValuesHandle:GetString &#x2013; Get a string from the keyvalues.
</div>
