---
template: lua-class-function.html
title: GetColor
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: KeyValuesHandle
  function: GetColor
  realm: shared
  description: "Get a color from the keyvalues."
  
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
    - type: Color
      description: "The value as a color."
---

<div class="lua__search__keywords">
KeyValuesHandle:GetColor &#x2013; Get a color from the keyvalues.
</div>
