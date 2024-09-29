---
template: lua-class-function.html
title: GetFloat
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: KeyValuesHandle
  function: GetFloat
  realm: shared
  description: "Get a float from the keyvalues."
  
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
        type: number
    default: "0.0f"
      - name: "default"
        type: number
    default: "0.0f"
  returns:
    - type: number
      description: "The value as a float."
---

<div class="lua__search__keywords">
KeyValuesHandle:GetFloat &#x2013; Get a float from the keyvalues.
</div>
