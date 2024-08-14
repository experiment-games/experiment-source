---
template: lua-library-function.html
title: CreateExtended
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: NetworkStringTables
  function: CreateExtended
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
    - name: "isFilenames"
      type: boolean
      default: "false"
  returns:
    - type: NetworkStringTable
      description: "The created string table."
---

<div class="lua__search__keywords">
NetworkStringTables.CreateExtended &#x2013; Create a new string table.
</div>
