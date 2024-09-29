---
template: lua-library-function.html
title: CreateFromTable
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: KeyValues
  function: CreateFromTable
  realm: shared
  description: "Create a new keyvalues object from a table."
  
  arguments:
  - position: 1
    name: "name"
    type: string
  - position: 2
    name: "table"
    type: table
  returns:
    - type: KeyValuesHandle
      description: "The new keyvalues object."
---

<div class="lua__search__keywords">
KeyValues.CreateFromTable &#x2013; Create a new keyvalues object from a table.
</div>
