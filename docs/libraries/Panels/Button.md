---
template: lua-library-function.html
title: Button
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panels
  function: Button
  realm: client
  description: "Creates a new button"
  
  arguments:
  - position: 1
    name: "parent"
    type: Panel
    default: "VGui_GetClientLuaRootPanel()"
  - position: 2
    name: "name"
    type: string
    default: "\"Button\""
  - position: 3
    name: "text"
    type: string
    default: "\"\""
  - position: 4
    name: "actionSignalTarget"
    type: Panel
    default: "0"
  - position: 5
    name: "cmd"
    type: string
    default: "0"
  returns:
    - type: Button
      description: "The new button"
---

<div class="lua__search__keywords">
Panels.Button &#x2013; Creates a new button
</div>
