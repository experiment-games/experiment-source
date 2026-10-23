---
template: lua-library-function.html
title: GetClientConsoleVariableValueAsNumber
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Engines
  function: GetClientConsoleVariableValueAsNumber
  realm: client
  description: "Get the value of a client console variable as a number."
  
  arguments:
  - position: 1
    types:
      - name: "playerOrIndex"
        type: Player
      - name: "playerOrIndex"
        type: number
  - position: 2
    name: "varName"
    type: string
  returns:
    - type: number
      description: "The value of the client console variable as a number."
---

<div class="lua__search__keywords">
Engines.GetClientConsoleVariableValueAsNumber &#x2013; Get the value of a client console variable as a number.
</div>
