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
  - position: 3
    types:
      - name: "default"
        type: string
    default: "\"\""
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
