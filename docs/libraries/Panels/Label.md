---
template: lua-library-function.html
title: Label
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panels
  function: Label
  realm: client
  description: "Creates a new label"
  
  arguments:
  - position: 1
    name: "parent"
    type: Panel
    default: "VGui_GetClientLuaRootPanel()"
  - position: 2
    name: "text"
    type: string
    default: "\"\""
  - position: 3
    name: "name"
    type: string
    default: "\"Label\""
  returns:
    - type: Label
      description: "The new label"
---

<div class="lua__search__keywords">
Panels.Label &#x2013; Creates a new label
</div>
