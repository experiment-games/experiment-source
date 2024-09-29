---
template: lua-class-function.html
title: AddString
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: NetworkStringTable
  function: AddString
  realm: shared
  description: "Add a string to the table."
  
  arguments:
  - position: 1
    name: "networkStringTable"
    type: NetworkStringTable
  - position: 2
    name: "isServer"
    type: boolean
  - position: 3
    name: "value"
    type: string
  returns:
    - type: integer
      description: "The index of the string."
---

<div class="lua__search__keywords">
NetworkStringTable:AddString &#x2013; Add a string to the table.
</div>
