---
template: lua-library-function.html
title: GetClientConsoleVariableValue
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Engines
  function: GetClientConsoleVariableValue
  realm: server
  description: "Get a client's ConsoleVariable value."
  
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
    - type: string
      description: "The client's ConsoleVariable value."
---

<div class="lua__search__keywords">
Engines.GetClientConsoleVariableValue &#x2013; Get a client's ConsoleVariable value.
</div>
