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
    - name: "parent"
      type: Panel
      default: "VGui_GetClientLuaRootPanel()"
    - name: "name"
      type: string
      default: "\"Button\""
    - name: "text"
      type: string
      default: "\"\""
    - name: "actionSignalTarget"
      type: Panel
      default: "0"
    - name: "cmd"
      type: string
      default: "0"
  returns:
    - type: Button
      description: "The new button"
---

<div class="lua__search__keywords">
Panels.Button &#x2013; Creates a new button
</div>
