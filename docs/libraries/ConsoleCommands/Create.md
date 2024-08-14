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
  library: ConsoleCommands
  function: Create
  realm: shared
  description: "Creates a console command or returns the existing one with the given name"
  
  arguments:
    - name: "name"
      type: string
    - name: "helpString"
      type: string
      default: "\"\""
    - name: "flags"
      type: number
      default: "0"
  returns:
    - type: ConsoleCommand
      description: "Created console command (returns existing command if one with the same name exists)"
---

<div class="lua__search__keywords">
ConsoleCommands.Create &#x2013; Creates a console command or returns the existing one with the given name
</div>
