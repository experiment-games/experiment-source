---
template: lua-library-function.html
title: GetClientConsoleVariableValue
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Engines
  function: GetClientConsoleVariableValue
  realm: client
  description: "Get the value of a client console variable."
  
  argumentSets:
    - arguments:
      - name: "playerOrIndex"
        type: Player
    - arguments:
      - name: "playerOrIndex"
        type: integer
      - name: "varName"
        type: string
  returns:
    - type: string
      description: "The value of the client console variable."
---

<div class="lua__search__keywords">
Engines.GetClientConsoleVariableValue &#x2013; Get the value of a client console variable.
</div>
