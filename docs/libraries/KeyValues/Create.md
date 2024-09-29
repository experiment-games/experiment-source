---
template: lua-library-function.html
title: Create
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: KeyValues
  function: Create
  realm: shared
  description: "Create a new keyvalues object."
  
  arguments:
  - position: 1
    name: "name"
    type: string
  - position: 2
    name: "firstKey"
    type: string
    nillable: true
  - position: 3
    types:
      - name: "firstValue"
        type: number
    nillable: true
      - name: "firstValue"
        type: string
    nillable: true
      - name: "firstValue"
        type: number
    nillable: true
      - name: "firstValue"
        type: string
    nillable: true
  - position: 4
    types:
      - name: "secondKey"
        type: string
    nillable: true
      - name: "secondKey"
        type: string
    nillable: true
  - position: 5
    types:
      - name: "secondValue"
        type: number
    nillable: true
      - name: "secondValue"
        type: string
    nillable: true
  returns:
    - type: KeyValuesHandle
      description: "The new keyvalues object."
---

<div class="lua__search__keywords">
KeyValues.Create &#x2013; Create a new keyvalues object.
</div>
