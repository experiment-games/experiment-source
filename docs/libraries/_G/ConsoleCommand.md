---
template: lua-library-function.html
title: ConsoleCommand
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: _G
  function: ConsoleCommand
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
_G.ConsoleCommand &#x2013; Creates a console command or returns the existing one with the given name
</div>
