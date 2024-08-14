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
  library: NetworkStringTables
  function: Create
  realm: shared
  description: "Create a new string table."
  
  arguments:
    - name: "name"
      type: string
    - name: "maxEntries"
      type: number
    - name: "userDataFixedSize"
      type: number
      default: "0"
    - name: "userDataNetworkBits"
      type: number
      default: "0"
  returns:
    - type: NetworkStringTable
      description: "The created string table."
---

<div class="lua__search__keywords">
NetworkStringTables.Create &#x2013; Create a new string table.
</div>
