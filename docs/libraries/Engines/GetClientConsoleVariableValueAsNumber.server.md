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
  
  argumentSets:
    - arguments:
      - name: "playerOrIndex"
        type: Player
    - arguments:
      - name: "playerOrIndex"
        type: integer
      - name: "conVarName"
        type: string
  returns:
    - type: number
      description: "The client's ConsoleVariable value as a number."
---

<div class="lua__search__keywords">
Engines.GetClientConsoleVariableValueAsNumber &#x2013; Get a client's ConsoleVariable value as a number.
</div>
