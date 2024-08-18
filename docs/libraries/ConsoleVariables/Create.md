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
  library: ConsoleVariables
  function: Create
  realm: shared
  description: "Creates a console variable or returns the existing one with the given name"
  
  arguments:
    - name: "name"
      type: string
    - name: "value"
      type: string
    - name: "flags"
      type: number
      default: "0"
    - name: "helpText"
      type: string
      default: "\"\""
    - name: "minimum"
      type: number
      default: "0.0"
    - name: "maximum"
      type: number
      default: "0"
  returns:
    - type: ConsoleVariable
      description: "The ConsoleVariable created or found"
---

<div class="lua__search__keywords">
ConsoleVariables.Create &#x2013; Creates a console variable or returns the existing one with the given name
</div>
