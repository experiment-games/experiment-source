---
template: lua-library-function.html
title: GetClientConsoleVariableValueAsNumber
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Engines
  function: GetClientConsoleVariableValueAsNumber
  realm: server
  description: "Get a client's ConsoleVariable value as a number."
  
  arguments:
  - position: 1
    types:
      - name: "playerOrIndex"
        type: Player
      - name: "playerOrIndex"
        type: number
  - position: 2
    name: "conVarName"
    type: string
  returns:
    - type: integer
      description: "The client's ConsoleVariable value as a number."
---

<div class="lua__search__keywords">
Engines.GetClientConsoleVariableValueAsNumber &#x2013; Get a client's ConsoleVariable value as a number.
</div>
